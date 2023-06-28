#include "MidiMaster.h"
#include "qmidimessage.h"
#include "qmidiout.h"
#include "qmidiin.h"

#include "View/QAN1xEditor.h"
#include <array>

QMidiOut* s_out{ nullptr };
QMidiIn* s_in{ nullptr };

QAN1xEditor* s_view{ nullptr };

bool handlingMessage = false;

int s_kbdOctave{ 5 };

void handleSysMsg(const Message& msg)
{
	if (msg.size() > 20)  //bulk
	{
		
		MidiMaster::syncBulk(msg);
		return;
	}

	//setting parameter to UI
	std::array<int, 4> header{ 240, 67, 16, 92 };

	for (int i = 0; i < header.size(); i++)
	{
		if (msg[i] != header[i]) return;
	}

	AN1x::ParamType type;

	if (msg[4] == 0) //System Param Change
	{
		type = AN1x::ParamType::System;
	}
	else
	{
		switch (msg[5])
		{
		case 0: type = AN1x::ParamType::Common; break;
		case 16: type = AN1x::ParamType::Scene1; break;
		case 17: type = AN1x::ParamType::Scene2; break;
		case 14: type = AN1x::ParamType::StepSq; break;
		default: return;
		}
	}

	unsigned char param = msg[6];

	if (AN1x::isNull(type, param)) return;

	int value = msg[7];

	if (AN1x::isTwoByteParameter(type, param))
	{
		value = msg[7] * 128;
		value += msg[8];
	}

	value -= AN1x::getOffset(type, param);

	s_view->setParameter(type, param, value);

	handlingMessage = false;
}

void sendMessage(const Message& msg)
{
	if (s_out == nullptr) return;

	if (handlingMessage) return;

	try {
		if (!s_out->isPortOpen()) return;

		auto m = msg;

		s_out->sendRawMessage(m);
	}
	catch (std::exception)
	{
		MidiMaster::refreshConnection();
	}


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

	QObject::connect(s_in, &QMidiIn::midiMessageReceived, s_view, [=](QMidiMessage* m)
	{
			//qDebug() << m->getRawMessage();

			handlingMessage = true;

			switch (m->getStatus())
			{
				case QMidiStatus::MIDI_PROGRAM_CHANGE: handlingMessage = false;  syncBulk(); break;
				case QMidiStatus::MIDI_SYSEX: handleSysMsg(m->getSysExData()); break;
				case QMidiStatus::MIDI_CONTROL_CHANGE: s_view->setModWheel(m->getRawMessage()[2]); break;
				default: handlingMessage = false; s_out->sendMessage(m);
			}

			handlingMessage = false;

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

void MidiMaster::modWheelChange(int value)
{
	if (value < 0 || value > 127) return;

	sendMessage({ 0xB0, 0x01, (unsigned char)value });
}

void MidiMaster::pitchChange(int value)
{
	if (value < 0 || value > 127) return;

	sendMessage({ 0xE0, 0x00, (unsigned char)value });
}


void MidiMaster::goToVoice(int value)
{
	if (value < 0 || value > 127) return;

	sendMessage({ 0xC0, (unsigned char)value });

	syncBulk();
}

void MidiMaster::syncBulk(const Message& m)
{
	if (s_out == nullptr || !s_out->isPortOpen()) return;
	if (s_in == nullptr || !s_in->isPortOpen()) return;

	auto handleBulk = [](AN1x::ParamType type, const std::vector<unsigned char>& paramList, int maxSize) {

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

			s_view->setParameter(type, param, value);
		}

	};

	static int syncCounter = 0;
	
	if (m.empty() && syncCounter != 0) { syncCounter = 0; }

	switch (syncCounter)
	{
		case 0:
			handlingMessage = false;
			sendMessage({ 0xF0, 0x43, 0x20, 0x5C, 0x00, 0x00, 0x00, 0xF7 }); break; //request System
		case 1: 
			handleBulk(AN1x::ParamType::System, m, AN1x::SystemMaxSize);
			handlingMessage = false;
			sendMessage({ 0xF0, 0x43, 0x20, 0x5C, 0x10, 0x00, 0x00, 0xF7 }); break; //request Common
		case 2: 
			handleBulk(AN1x::ParamType::Common, m, AN1x::CommonMaxSize);
			handlingMessage = false;
			sendMessage({ 0xF0, 0x43, 0x20, 0x5C, 0x10, 0x10, 0x00, 0xF7 }); break; //request Scene1
		case 3: 
			handleBulk(AN1x::ParamType::Scene1, m, AN1x::SceneParametersMaxSize);
			handlingMessage = false;
			sendMessage({ 0xF0, 0x43, 0x20, 0x5C, 0x10, 0x11, 0x00, 0xF7 }); break; //request Scene2
		case 4:
			handleBulk(AN1x::ParamType::Scene2, m, AN1x::SceneParametersMaxSize);
			handlingMessage = false;
			sendMessage({ 0xF0, 0x43, 0x20, 0x5C, 0x10, 0x0E, 0x00, 0xF7 }); break; //request SeqPattern
		case 5: //termination
			handleBulk(AN1x::ParamType::StepSq, m, AN1x::StepSequencerMaxSize);
			handlingMessage = false;
			syncCounter = 0;
			return;
		default:
			syncCounter = 0; return;
	}	

	syncCounter++;

}

void MidiMaster::setView(QAN1xEditor* v) {

	s_view = v;
}

void MidiMaster::connectMidiIn(int idx)
{
	if (!s_in) return;

	s_in->closePort();
	
	if(idx != -1)
		s_in->openPort(idx);
}

void MidiMaster::connectMidiOut(int idx)
{
	if (!s_out) return;

	s_out->closePort();

	if(idx != -1)
		s_out->openPort(idx);
}



//MIDI Keyboard



void MidiMaster::setKbdOctave(int octave) {
	s_kbdOctave = octave + 2;
}



void MidiMaster::pcKeyPress(int kbd_key, bool pressed, int velocity) {

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
	186, //Qt::Key_Semicolon,
	222, //Qt::Key_Apostrophe,
	221, //Qt::Key_BracketRight,
	220 //Qt::Key_Backslash
	};

	int note{ -1 };

	for (int i = 0; i < 20; i++) {
		if (kbd_key == s_buttonsNotes[i]) {
			note = (12 * s_kbdOctave) + i;

			if(note > 127) note = -1;
		}
	}

	setNote(note, pressed, velocity);
};


void MidiMaster::setNote(int note, bool on, int velocity) {

	if (note == -1) return;

	QMidiMessage* m = new QMidiMessage();

	m->setPitch(note);
	m->setStatus(on ? QMidiStatus::MIDI_NOTE_ON : QMidiStatus::MIDI_NOTE_OFF);
	m->setVelocity(velocity);

	s_out->sendMessage(m);

	s_view->pianoRoll()->setNote(note, on);
};

