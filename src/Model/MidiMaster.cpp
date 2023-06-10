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

bool handlingSysMsg = false;
bool sendingMessage = false;
int sync_num = -1;
int s_kbdOctave{ 5 };

std::array<std::vector<unsigned char>, 4> s_voiceState;

void handleSysMsg(const Message& msg)
{
	handlingSysMsg = true;

	if (sync_num != -1) {

		s_voiceState[sync_num] = msg;

		handlingSysMsg = false;
		MidiMaster::requestBulk();

		return;
	}

	handlingSysMsg = false;
}

void sendMessage(const Message& msg)
{
	if (s_out == nullptr) return;

	if (handlingSysMsg) return;

	sendingMessage = true;

	try {
		if (!s_out->isPortOpen()) return;

		auto m = msg;

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
			if (m->getStatus() == QMidiStatus::MIDI_SYSEX)
			{
				handleSysMsg(m->getSysExData()); 
			}
			else
			{
				s_out->sendMessage(m);
			}

			delete m;
			
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

void MidiMaster::requestBulk()
{
	if (s_out == nullptr || !s_out->isPortOpen()) return;
	if (s_in == nullptr || !s_in->isPortOpen()) return;

	sync_num++;

	switch (sync_num)
	{
		case 0: 
			sendMessage({ 0xF0, 0x43, 0x20, 0x5C, 0x10, 0x00, 0x00, 0xF7 }); return; //Common
		case 1: 
			sendMessage({ 0xF0, 0x43, 0x20, 0x5C, 0x10, 0x10, 0x00, 0xF7 }); return; //Scene1
		case 2: sendMessage({ 0xF0, 0x43, 0x20, 0x5C, 0x10, 0x11, 0x00, 0xF7 }); return ; //Scene2
		case 3: sendMessage({ 0xF0, 0x43, 0x20, 0x5C, 0x10, 0x0E, 0x00, 0xF7 }); return; //SeqPattern
	}

	sync_num = -1;

	auto lambda = [&](AN1x::ParamType type, const std::vector<unsigned char>& paramList, int maxSize) {

		constexpr int header = 9;

		//header check
		if (paramList[0] != 0xF0) return;
		if (paramList[1] != 0x43) return;
		if (paramList[3] != 0x5C) return;

		if (paramList.size() < maxSize + header) return;

		for (int i = header; i < maxSize + header; i++)
		{
			unsigned char param = i - header;

			if (AN1x::isNull(type, param)) continue;
			int value = paramList[i];

			if (AN1x::isTwoByteParameter(type, param))
			{
				value = paramList[i] * 128;
				value += paramList[i + 1];

				i++;
			}

			value -= AN1x::getOffset(type, param);

			qDebug() << (int)type << (int)param << (int)value;

			s_view->setParameter(type, param, value);
		}

	};

	lambda(AN1x::ParamType::Common, s_voiceState[0], AN1x::CommonMaxSize);
	lambda(AN1x::ParamType::Scene1, s_voiceState[1], AN1x::SceneParametersMaxSize);
	lambda(AN1x::ParamType::Scene2, s_voiceState[2], AN1x::SceneParametersMaxSize);
	lambda(AN1x::ParamType::StepSq, s_voiceState[3], AN1x::StepSequencerMaxSize);
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



void MidiMaster::setKbdOctave(int octave) {
	s_kbdOctave = octave + 2;
}



void MidiMaster::pcKeyPress(int kbd_key, bool pressed) {

	static int s_buttonsNotes[20]{
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

	int note{ -1 };

	for (int i = 0; i < 20; i++) {
		if (kbd_key == s_buttonsNotes[i]) {
			note = (12 * s_kbdOctave) + i;

			if(note > 127) note = -1;
		}
	}

	qDebug() << note;

	setNote(note, pressed);
};


void MidiMaster::setNote(int note, bool on) {

	if (note == -1) return;

	QMidiMessage* m = new QMidiMessage();

	m->setPitch(note);
	m->setStatus(on ? QMidiStatus::MIDI_NOTE_ON : QMidiStatus::MIDI_NOTE_OFF);
	m->setVelocity(127);

	s_out->sendMessage(m);

	s_view->pianoRoll()->setNote(note, on);
};

