#include "MidiMaster.h"
#include "qmidimessage.h"
#include "qmidiout.h"
#include "qmidiin.h"

#include "View/QAN1xEditor.h"
#include "An1xPatch.h"
#include "PatchMemory.h"
#include <array>

QMidiOut* s_out{ nullptr };
QMidiIn* s_in{ nullptr };

QAN1xEditor* s_view{ nullptr };

An1xPatch current_patch;

bool handlingMessage = false;

int s_kbdOctave{ 5 };

void handleSysMsg(const Message& msg)
{

	if (msg.size() == 1953)  //bulk
	{
		handlingMessage = false;
		PatchMemory::patchRecieved({ msg });
		//s_view->setPatch(current_patch);
		return;
	}

	if (msg.size() == 39) //system
	{
		current_patch.setSystemData(msg);
		
		for (int i = 0; i < AN1x::SystemMaxSize; i++)
		{
			s_view->setParameter(AN1xParam::Type::System, i, current_patch.getParameter(AN1xParam::Type::System, i));
		}

		return;
	}

	//setting parameter to UI
	std::array<int, 4> header{ 240, 67, 16, 92 };

	for (int i = 0; i < header.size(); i++)
	{
		if (msg[i] != header[i]) return;
	}

	AN1xParam::Type type;

	if (msg[4] == 0) //System Param Change
	{
		type = AN1xParam::Type::System;
	}
	else
	{
		switch (msg[5])
		{
		case 0: type = AN1xParam::Type::Common; break;
		case 16: type = AN1xParam::Type::Scene1; break;
		case 17: type = AN1xParam::Type::Scene2; break;
		case 14: type = AN1xParam::Type::StepSq; break;
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

	current_patch.setParameter(type, param, value);
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
			handlingMessage = true;

			switch (m->getStatus())
			{
				case QMidiStatus::MIDI_PROGRAM_CHANGE: handlingMessage = false; break;
				case QMidiStatus::MIDI_SYSEX: handleSysMsg(m->getSysExData()); break;
				case QMidiStatus::MIDI_CONTROL_CHANGE: s_view->setModWheel(m->getRawMessage()[2]); break;
				default: handlingMessage = false; s_out->sendMessage(m);
			}

			handlingMessage = false;

			delete m;
	});

	s_view->setMidiDevices(s_in->getPorts(), s_out->getPorts());
	
}

void MidiMaster::setParam(AN1xParam::Type type, unsigned char parameter, int value)
{
	sendMessage(current_patch.setParameter(type, parameter, value));
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
	//sendMessage({ 0xC0, (unsigned char)value });
	sendMessage(AN1x::voiceRequest(value));

	//syncBulk();
}

void MidiMaster::requestVoice(int index)
{
	if (index < 0 || index > 127) return;
	sendMessage(AN1x::voiceRequest(index));
}

void MidiMaster::requestSystem()
{
	sendMessage({ 0xF0, 0x43, 0x20, 0x5C, 0x00, 0x00, 0x00, 0xF7 });
}

void MidiMaster::syncBulk(const Message& m)
{
	if (s_out == nullptr || !s_out->isPortOpen()) return;
	if (s_in == nullptr || !s_in->isPortOpen()) return;
	
	//sendMessage({ 0xF0, 0x43, 0x20, 0x5C, 0x00, 0x00, 0x00, 0xF7 }); break; //request System

	//current_patch.setBulkPatch(m);
	//s_view->setPatch(current_patch);


	return;
}

void MidiMaster::setCurrentPatch(const An1xPatch& p)
{
	s_view->setPatch(p);
}


void MidiMaster::sendCommonBulk()
{
	if (handlingMessage) return;

	std::vector<unsigned char> result{ 0xF0, 0x43, 0x00, 0x5C, 0x0C, 0x68, 0x10, 0x00, 0x00 };

	auto bulkData = s_view->getCommon();

	for (int i = 0; i < bulkData.size(); i++)
	{
		if (i > AN1x::CommonParam::FreeEgTrackSceneSw4) continue;

		if(AN1x::isNull(AN1xParam::Type::Common, i)) continue;

		bulkData[i] += AN1x::getOffset(AN1xParam::Type::Common, i);

		if (AN1x::isTwoByteParameter(AN1xParam::Type::Common, i)) {

			auto value = bulkData[i];
			bulkData[i] = value / 128;
			i++;
			bulkData[i] = value % 128;

		}
	}
	
	result.insert(result.end(), bulkData.begin(), bulkData.end());

	unsigned char sum = 0x00;

	for (int i = 4; i < result.size(); i++) {
		
		sum += result[i]; //intended arithmetic overflow
	}

	unsigned char checkSum = 128 - sum;
	if (checkSum >= 128) checkSum = checkSum - 128;
	result.push_back(checkSum);

	unsigned char controlDebug{ 0 };

	for (int i = 4; i < result.size(); i++)
	{
		controlDebug += result[i];
	}

	result.push_back(0xF7);

	sendMessage(result);
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

