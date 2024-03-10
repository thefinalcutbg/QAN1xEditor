#pragma once

#include "An1x.h"

#include <vector>

class QAN1xEditor;
class AN1xPatch;

struct PatchSource {

	enum Location { Database, SynthMemory };

	Location location{ Location::Database };
	long long id{ 0 };

	long long getRowid() const {

		return location == Location::SynthMemory ? 0 : id;

		return id;
	}

	int getMemoryIndex() const {
		return location == Location::Database ? -1 : id;
	}
};


namespace MidiMaster
{
	void setView(QAN1xEditor* v);

	//MIDI connectivity functions
	void refreshConnection();
	void connectMidiIn(int idx);
	void connectMidiOut(int idx);

	//called when parameter is changed from UI
	void parameterChanged(ParamType type, unsigned char parameter, int value);
	//called when Free EG Track change from UI
	void FreeEGChanged(const std::vector<int>& trackData);

	void requestSystem();
	void sendSystem();
	void restoreSystem();

	//used by PatchMemory when requesting voices from AN1x 
	void requestVoice(int index);
    void sendBulk(const AN1xPatch& patch, int index);

	//setting patch to view
	void setCurrentPatch(const AN1xPatch& p, PatchSource src);
	const AN1xPatch& currentPatch();
	void notifyRowidDelete(long long rowid);
	void newPatch(AN1x::InitType type);

	//PC keyboard as Midi
	void modWheelChange(int value);
	void pitchChange(int value);
	void setKbdOctave(int octave);
	void pcKeyPress(int pcKey, bool pressed, int velocity); 	//pressed = false means released
	void setNote(int note, bool press, int velocity); 	//pressed = false means released
    void setSendChannel(int channel);
	void stopAllSounds();
    void cleanUp();
}
