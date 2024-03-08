#include "MidiMaster.h"
#include "QMidi/qmidimessage.h"
#include "QMidi/qmidiout.h"
#include "QMidi/qmidiin.h"
#include "View/QAN1xEditor.h"
#include "An1xPatch.h"
#include "View/GlobalWidgets.h"
#include "PatchMemory.h"
#include "PatchDatabase.h"
#include <array>
#include <QtGlobal>
#include <QTimer>
#include <QEventLoop>

void waitForAWhile()
{
    QEventLoop loop;
    QTimer::singleShot(500, &loop, SLOT(quit()));
    loop.exec();
}

//static variables
QMidiOut* s_out{ nullptr };
QMidiIn* s_in{ nullptr };

QAN1xEditor* s_view{ nullptr };

//patch currently edited
AN1xPatch current_patch;
PatchSource patch_src;
bool is_edited{ false };

int s_kbdOctave{ 5 };
int s_sendChannel{1};

//guards against recursion when setting parameters to view
bool handlingMessage = true;

void MidiMaster::setView(QAN1xEditor* v) {

	s_view = v;

	handlingMessage = true;

	s_view->setPatch(current_patch);

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
	catch (std::exception) { 
		MidiMaster::refreshConnection();
	}
}

void handleSysMsg(const Message& msg)
{
	if (msg.size() == 1953)  //voice bulk recieved
	{
		handlingMessage = false;
		PatchMemory::patchRecieved({ msg });
		return;
	}

	if (msg.size() == 39) //system data recieved
	{
		current_patch.setSystemData(msg);

		for (int i = 0; i < AN1x::SystemMaxSize; i++)
		{
			s_view->setParameter(ParamType::System, i, current_patch.getParameter(ParamType::System, i));
		}

		return;
	}

	//parameter recieved (AN1x doesnt send AEG ADSR and a few more)
	std::array<int, 4> header{ 240, 67, 16, 92 };

    for (size_t i = 0; i < header.size(); i++)
	{
		if (msg[i] != header[i]) return;
	}

	ParamType type;

	if (msg[4] == 0) //System Param Change
	{
		type = ParamType::System;
	}
	else
	{
		switch (msg[5])
		{
		case 0: type = ParamType::Common; break;
		case 16: type = ParamType::Scene1; break;
		case 17: type = ParamType::Scene2; break;
		case 14: type = ParamType::StepSq; break;
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
	is_edited = true;


	handlingMessage = false;
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

void MidiMaster::connectMidiIn(int idx)
{
	if (!s_in) return;

	s_in->closePort();

	if (idx != -1)
		s_in->openPort(idx);
}

void MidiMaster::connectMidiOut(int idx)
{
	if (!s_out) return;

	s_out->closePort();

	if (idx != -1)
		s_out->openPort(idx);
}


void MidiMaster::parameterChanged(ParamType type, unsigned char parameter, int value)
{
	if (handlingMessage) return;

	is_edited = true;

	sendMessage(current_patch.setParameter(type, parameter, value));
}


void MidiMaster::FreeEGChanged(const std::vector<int>& trackData)
{
	current_patch.setFreeEGData(trackData);

	is_edited = true;

	//sending the whole common bulk
	sendMessage(current_patch.getDataMessage(ParamType::Common));
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

/*
void MidiMaster::goToVoice(int value)
{
	if (value < 0 || value > 127) return;
	//sendMessage({ 0xC0, (unsigned char)value });
	sendMessage(AN1x::voiceRequest(value));

	//syncBulk();
}
*/

void MidiMaster::requestVoice(int index)
{
	if (index < 0 || index > 127) return;
	sendMessage(AN1x::voiceRequest(index));
}

void MidiMaster::sendBulk(const AN1xPatch& patch, int idx)
{
    if (idx < 0 || idx > 127) return;

    Message msg = { 0xF0, 0x43, 0x00, 0x5C, 0x0F, 0x16, 0x11, (unsigned char)idx, 0x00 };

    msg.reserve(patch.rawData().size() + 11);

    for (auto value : patch.rawData()) msg.push_back(value);

    AN1x::addCheckSum(msg);

    sendMessage(msg);

    waitForAWhile();

	handlingMessage = false;

    PatchMemory::patchSent();
}

void MidiMaster::requestSystem()
{
	sendMessage({ 0xF0, 0x43, 0x20, 0x5C, 0x00, 0x00, 0x00, 0xF7 });
}

void MidiMaster::sendSystem()
{
	sendMessage(AN1xPatch::getSystemDataMsg());
}

void MidiMaster::restoreSystem()
{
	AN1xPatch::restoreSystemData();

	sendSystem();

	handlingMessage = true;

	for (int i = 0; i < AN1xPatch::SystemSize; i++)
	{
		s_view->setParameter(ParamType::System, i, current_patch.getParameter(ParamType::System, i));
	}

	handlingMessage = false;
	
}

void MidiMaster::setCurrentPatch(const AN1xPatch& p, PatchSource src)
{
	if (src.getRowid() != 0 && src.getRowid() == patch_src.getRowid()) return;

	if (is_edited &&
		GlobalWidgets::askQuestion("Do you want to save current patch?")
	)
	{
		if (patch_src.location == PatchSource::Database) {
			PatchDatabase::saveVoice(current_patch, patch_src.getRowid());
		}
		else {
			PatchMemory::setPatch(current_patch, patch_src.getMemoryIndex());
		}
	}

	current_patch = p;

	is_edited = false;

	patch_src = src;

	handlingMessage = true;

	s_view->setPatch(p);

	handlingMessage = false;

	sendMessage(p.getDataMessage(ParamType::Common)); //Sleep(100);
	sendMessage(p.getDataMessage(ParamType::Scene1)); //Sleep(100);
	sendMessage(p.getDataMessage(ParamType::Scene2)); //Sleep(100);
	sendMessage(p.getDataMessage(ParamType::StepSq)); //Sleep(100);
}

const AN1xPatch& MidiMaster::currentPatch()
{
	return current_patch;
}

void MidiMaster::notifyRowidDelete(long long rowid)
{
	if (patch_src.getRowid() != rowid) return;

	patch_src.id = 0;

	is_edited = true;
}

void MidiMaster::newPatch(AN1x::InitType type)
{
	setCurrentPatch(AN1xPatch(type), { PatchSource::Database, 0 });
}


void MidiMaster::setKbdOctave(int octave) {
	stopAllSounds();
	s_kbdOctave = octave + 2;
}

void MidiMaster::pcKeyPress(int kbd_key, bool pressed, int velocity) {

	static int s_buttonsNotes[20]{
            Qt::Key_A,
            Qt::Key_W,
            Qt::Key_S,
            Qt::Key_E,
            Qt::Key_D,
            Qt::Key_F,
            Qt::Key_T,
            Qt::Key_G,
            Qt::Key_Y,
            Qt::Key_H,
            Qt::Key_U,
            Qt::Key_J,
            Qt::Key_K,
            Qt::Key_O,
            Qt::Key_L,
            Qt::Key_P,
            Qt::Key_Semicolon,
            Qt::Key_Apostrophe,
            Qt::Key_BracketRight,
            Qt::Key_Backslash,
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
    m->setChannel(s_sendChannel);
	s_out->sendMessage(m);
    qDebug() << m->getChannel();
	s_view->pianoRoll()->setNote(note, on);

}

void MidiMaster::stopAllSounds()
{
	for (unsigned char channel = 176; channel < 192; channel++) 
	{
		sendMessage({ channel, 0x78, 0x00 });
		sendMessage({ channel, 0x79, 0x00 });
	}
}



void MidiMaster::setSendChannel(int channel)
{
    if(channel < 1 || channel > 16) return;
    stopAllSounds();
    s_sendChannel = channel;
}
