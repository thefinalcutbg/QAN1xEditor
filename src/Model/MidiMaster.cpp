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

void waitForAWhile(int ms = 500)
{
    QEventLoop loop;
    QTimer::singleShot(ms, &loop, SLOT(quit()));
    loop.exec();
}

//static variables
QMidiOut* s_out{ nullptr };
QMidiIn* s_in{ nullptr };
AdvancedMidiSettings settings;

QAN1xEditor* s_view{ nullptr };

//patch currently edited
AN1xPatch current_patch;
PatchSource current_patch_src;
bool s_voice_edited{ false };

int s_kbdOctave{ 5 };

//guards against recursion when setting parameters to view
bool handlingMessage = true;

void MidiMaster::setView(QAN1xEditor* v) {

	s_view = v;

	handlingMessage = true;

	s_view->setPatch(current_patch);

	handlingMessage = false;

	refreshConnection();

	auto settings = PatchDatabase::getMidiSettings();

	setAdvancedSettings(settings.second);

	s_view->setMidiDeviceNames(settings.first);

	
}

void saveCurrentSettings() {

	PatchDatabase::setMidiSettings(s_view->getCurrentDevices(), settings);
}

void makeEdited(bool edited) {
	s_voice_edited = edited;
	s_view->enableSaveButton(edited);
}

//returns false if dialog is CANCELED
bool permissionToChangePatch() {

	if (!s_voice_edited) return true;

	std::string question = "Do you want to save current patch - ";
	question += current_patch.getName();
	
	while (question.at(question.size() - 1) == ' ') {
		question.pop_back();
	}
	question += "?";

	int answer = GlobalWidgets::YesNoCancelQuestion(question.c_str());

	switch (answer) {
		//no
		case 0: return true;
		//yes
		case 1:
			MidiMaster::saveCurrentPatch();
			return true;
		//cancel
		default: return false;
	}

	return true;

}

void sendMessage(const Message& msg, bool bulkDump = false)
{
	if (s_out == nullptr) return;

	if (handlingMessage) return;

	try {
		if (!s_out->isPortOpen()) return;

		auto m = msg;

		//SYSEX
		if (m.size() > 4 &&
			m[0] == 0xF0 && 
			m[1] == 0x43 && 
			m[3] == 0x5C
		){
			m[2] += settings.device_no-1;
		}

		if(!bulkDump){
			s_out->sendRawMessage(m);
			return; 
		}

		AN1x::addCheckSum(m);

		m.push_back(0xF7);

		//send whole bulk dump at once
		if (!settings.buffer_size) {

			s_out->sendRawMessage(m);

			return;
		}

		const auto chunkSize = settings.buffer_size;

		auto chunks = m.size() / chunkSize;
		auto remainder = m.size() % chunkSize;

		for (size_t i = 0; i < chunks; ++i) {

			auto start = i * chunkSize;
			auto end = start + chunkSize;

			std::vector<unsigned char> chunk;

			chunk.insert(chunk.end(), m.begin() + start, m.begin() + end);

			s_out->sendRawMessage(chunk);

			waitForAWhile(settings.msDelay);
		}

		if (!remainder) return;

		//the last chunk

		auto start = chunks * chunkSize;
	
		std::vector<unsigned char> chunk;

		chunk.insert(chunk.end(), m.begin() + start, m.end());

		s_out->sendRawMessage(chunk);

	}
	catch (std::exception) { 
		MidiMaster::refreshConnection();
	}
}

void handleSysMsg(const Message& msg)
{
	//header check:
	if (   
		msg[0] != 0xF0 //exclusive
	 || msg[1] != 0x43 //YAMAHA ID
	 || msg[2]%16 != settings.device_no-1
	 || msg[3] != 0x5C //AN1x MODEL ID
	) {
		return;
	}

	if (msg.size() == 1953)  //voice bulk recieved
	{
		handlingMessage = false; //otherwise the request for the next patch won't be recieved 

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
        default:  return;
		}
	}

	unsigned char param = msg[6];

    if (AN1x::isNull(type, param)){
        return;
    }

	int value = msg[7];

	if (AN1x::isTwoByteParameter(type, param))
	{
		value = msg[7] * 128;
		value += msg[8];
	}

	value -= AN1x::getOffset(type, param);

	current_patch.setParameter(type, param, value);
	s_view->setParameter(type, param, value);

    if(type != ParamType::System){
        makeEdited(true);
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

		auto status = m->getStatus();

		if (status == QMidiStatus::MIDI_SYSEX) {
			handleSysMsg(m->getSysExData());
		}

		if (settings.midi_thru) {
			s_out->sendMessage(m);
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

    if(type != ParamType::System){
        makeEdited(true);
    }

	sendMessage(current_patch.setParameter(type, parameter, value));
}


void MidiMaster::FreeEGChanged(const std::vector<int>& trackData)
{
	current_patch.setFreeEGData(trackData);

	makeEdited(true);

	//sending the whole common bulk
	sendMessage(current_patch.getDataMessage(ParamType::Common));
}

void MidiMaster::setAdvancedSettings(const AdvancedMidiSettings& advSettings)
{
	if (settings.midi_send_channel != advSettings.midi_send_channel) {
		stopAllSounds();
	}

	settings = advSettings;

	saveCurrentSettings();
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

	sendMessage(msg, true);

    waitForAWhile(settings.msDelay);

	handlingMessage = false;

}

void MidiMaster::requestSystem()
{
	Message msg = { 0xF0, 0x43, 0x20, 0x5C, 0x00, 0x00, 0x00, 0xF7 };

	sendMessage(msg);
}

void MidiMaster::sendSystem()
{
	sendMessage(AN1xPatch::getSystemDataMsg(), true);
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
	if (s_voice_edited) {

		if (!current_patch_src.isSameAs(src)) {

			if(!permissionToChangePatch()) return;
		}
		else if(!GlobalWidgets::askQuestion("Do you want to reload the current patch and lose all changes?")){

			return;
		}
	}

	current_patch = p;

	makeEdited(false);

	current_patch_src = src;

	handlingMessage = true;

	s_view->setPatch(p);

	handlingMessage = false;

    sendMessage(p.getDataMessage(ParamType::Common), true); waitForAWhile(settings.msDelay);
    sendMessage(p.getDataMessage(ParamType::Scene1), true); waitForAWhile(settings.msDelay);
    sendMessage(p.getDataMessage(ParamType::Scene2), true); waitForAWhile(settings.msDelay);
    sendMessage(p.getDataMessage(ParamType::StepSq), true);
}

const AN1xPatch& MidiMaster::currentPatch()
{
	return current_patch;
}

void MidiMaster::notifyRowidDelete(long long rowid)
{
	if (current_patch_src.getRowid() != rowid) return;

	current_patch_src.id = 0;

	makeEdited(true);
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
    m->setChannel(settings.midi_send_channel);
	s_out->sendMessage(m);

	s_view->pianoRoll()->setNote(note, on);

}

void MidiMaster::modWheelChange(int value)
{
    if (value < 0 || value > 127) return;

    unsigned char channel = 0xB0 + settings.midi_send_channel - 1;
    sendMessage({ channel, 0x01, (unsigned char)value });
}

void MidiMaster::pitchChange(int value)
{
    if (value < 0 || value > 127) return;

    unsigned char channel = 0xE0 + settings.midi_send_channel - 1;
    sendMessage({ channel, 0x00, (unsigned char)value });
}


void MidiMaster::stopAllSounds()
{
    unsigned char channel = 176 + settings.midi_send_channel - 1;

    sendMessage({ channel, 0x78, 0x00 });

}

void MidiMaster::saveCurrentPatch()
{
	if (!s_voice_edited) return;

	current_patch_src.location == PatchSource::Database ?
		PatchDatabase::saveVoice(current_patch, current_patch_src.getRowid())
		:
		PatchMemory::setPatch(current_patch, current_patch_src.getMemoryIndex());

	makeEdited(false);

}

bool MidiMaster::cleanUp()
{
	MidiMaster::stopAllSounds();

	if (current_patch_src.location == PatchSource::Database) {
		if (!permissionToChangePatch()) return false;
	}

	saveCurrentSettings();

    if(s_in) { delete s_in; }
    if(s_out) { delete s_out; }

	return true;

}
