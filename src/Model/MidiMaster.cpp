#include "MidiMaster.h"
#include "qmidimessage.h"
#include "qmidiout.h"
#include "qmidiin.h"
#include <QDebug>
#include "View/QAN1xEditor.h"
#include <array>

QMidiOut* s_out{ nullptr };
QMidiIn* s_in{ nullptr };

QAN1xEditor* s_view{ nullptr };

bool handlingMessage = false;
bool sendingMessage = false;


bool handleCommonParameter(const Message& m)
{

	static const std::array<int, 6> header = { 240, 67, 16, 92, 16, 0 };

	bool isCommonParam{ true };

	for (int i = 0; i < header.size(); i++)
	{
		if (i >= m.size() || header[i] != m[i]) {
			isCommonParam = false;
			break;
		}
	}

	if (!isCommonParam) return false;

	AN1x::CommonParam parameter = static_cast<AN1x::CommonParam>(m[6]);

	if (AN1x::isNull(AN1x::ParamType::Common, parameter)) return false;

	int value = m[7];

	if (AN1x::isTwoByteParameter(AN1x::ParamType::Common, parameter))
	{
		value = m[7] * 128;
		value += m[8];
	}

	value -= AN1x::getOffset(AN1x::ParamType::Common, parameter);

	if (!s_view) return true;

	s_view->setCommonParameter(parameter, value);

	return true;
}

bool handleSceneParameter(const Message& m)
{

	static const std::array<int, 5> header = { 240, 67, 16, 92, 16 };


	for (int i = 0; i < header.size(); i++)
	{
		if (i >= m.size() || header[i] != m[i]) return false;
	}


	if (m[5] != 16 && m[5] != 17) return false;

	bool isScene2 = m[5] == 16 ? false : true;
	AN1x::SceneParam parameter = static_cast<AN1x::SceneParam>(m[6]);

	if (AN1x::isNull(AN1x::ParamType::Scene1, parameter)) return false;

	int value = m[7];

	if (AN1x::isTwoByteParameter(AN1x::ParamType::Scene1, parameter))
	{
		value = m[7] * 128;
		value += m[8];
	}

	value -= AN1x::getOffset(AN1x::ParamType::Scene1, parameter);

	if (!s_view) return true;

	s_view->setSceneParameter(parameter, value, isScene2);

	return true;

}

bool handleSequenceParameter(const Message& m)
{
	static const std::array<int, 6> header = { 240, 67, 16, 92, 16, 14 };

	for (int i = 0; i < header.size(); i++)
	{
		if (i >= m.size() || header[i] != m[i]) return false;
	}


	s_view->setSequenceParameter((AN1x::SeqParam)m[6], m[7]);

	return true;
}

bool handleGlobalParameter(const Message& m)
{
	return false;
}




void handleMessage(const Message& msg)
{
	qDebug() << "recieve: " << msg;
	handlingMessage = true;

	handleCommonParameter(msg);
	handleSceneParameter(msg);
	handleSequenceParameter(msg);

	handlingMessage = false;
}



void sendMessage(const Message& msg)
{
	if (s_out == nullptr) return;

	if (handlingMessage) return;

	sendingMessage = true;

	try {
		if (!s_out->isPortOpen()) return;

		auto m = msg;

		qDebug() << "send: " << m;

		s_out->sendRawMessage(m);
	}
	catch (std::exception)
	{
		MidiMaster::refreshConnection();
	}

	sendingMessage = false;
}


void MidiMaster::refreshConnection()
{
	if (s_out) {
		s_out->closePort();
		delete s_out;
	}

	if (s_in)
	{
		s_in->closePort();
		delete s_in;
	}
	
	s_out = new QMidiOut;
	s_in = new QMidiIn;

	s_in->setIgnoreTypes(false, true, true);

	QObject::connect(s_in, &QMidiIn::midiMessageReceived, [=](QMidiMessage* m) 
		{  

			handleMessage(m->getSysExData()); delete m;
			
		});

	s_view->setMidiDevices(s_in->getPorts(), s_out->getPorts());
	
}

void MidiMaster::setParam(AN1x::ParamType type, unsigned char parameter, int value)
{
	auto msg = AN1x::getHeader(type);

	msg.push_back(parameter);

	value += AN1x::getOffset(type, parameter);

	if (AN1x::isTwoByteParameter(type, parameter)) {
		msg.push_back(value / 128);
		msg.push_back(value % 128);
	}
	else {
		msg.push_back(value);
	}

	msg.push_back(0xF7);

	sendMessage(msg);
}

void MidiMaster::setView(QAN1xEditor* v) {

	s_view = v;
}

void MidiMaster::connectMidiIn(int idx)
{
	if (!s_in) return;

	if (idx == -1) return;

	s_in->openPort(idx);

}

void MidiMaster::connectMidiOut(int idx)
{
	if (!s_out) return;

	if (idx == -1) return;

	s_out->openPort(idx);
}



//MIDI Keyboard
int s_buttonsNotes[20]{
	65, //Qt::Key_A,
	87, //Qt::Key_W,
	83, //Qt::Key_S,
	69, //Qt::Key_E,
	68, //Qt::Key_D,
	70, //Qt::Key_F,
	84, //Qt::Key_T,
	71, //Qt::Key_G,
	89, //Qt::Key_Y,
	72, //Qt::Key_H,
	85, //Qt::Key_U,
	74, //Qt::Key_J,
	75, //Qt::Key_K,
	79, //Qt::Key_O,
	76, //Qt::Key_L,
	80, //Qt::Key_P,
	59, //Qt::Key_Semicolon,
	39, //Qt::Key_Apostrophe,
	93, //Qt::Key_BracketRight,
	92 //Qt::Key_Backslash
};
int s_octave{ 5 };

void MidiMaster::setKbdOctave(int octave) {
	
	s_octave = octave + 2;
}

int getNote(int key)
{
	for (int i = 0; i < 20; i++) {
		if (key == s_buttonsNotes[i]) {
			int note = (12 * s_octave) + i;

			return note < 128 ? note : -1;
		}
	}
	return -1;
}

void MidiMaster::pcKeyPress(int kbd_key) {

	noteOn(getNote(kbd_key));
};

void MidiMaster::pcKeyRelease(int kbd_key) {

	noteOff(getNote(kbd_key));
};

void MidiMaster::noteOn(int note) {

	if (note == -1) return;

	QMidiMessage* m = new QMidiMessage();

	m->setPitch(note);
	m->setStatus(QMidiStatus::MIDI_NOTE_ON);
	m->setVelocity(127);

	s_out->sendMessage(m);

	s_view->pianoRoll()->setNoteOn(note);
};

void MidiMaster::noteOff(int note) {

	if (note == -1) return;

	QMidiMessage* m = new QMidiMessage();

	m->setPitch(note);
	m->setStatus(QMidiStatus::MIDI_NOTE_OFF);
	m->setVelocity(127);

	s_out->sendMessage(m);

	s_view->pianoRoll()->setNoteOff(note);
	
};