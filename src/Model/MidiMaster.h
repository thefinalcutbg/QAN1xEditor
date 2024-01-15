#pragma once

#include "An1x.h"

#include <vector>

class QAN1xEditor;
class AN1xPatch;


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
	void sendBulk(const Message& m);

	//setting patch to view
	void setCurrentPatch(const AN1xPatch& p);

	//PC keyboard as Midi
	void modWheelChange(int value);
	void pitchChange(int value);
	void setKbdOctave(int octave);
	void pcKeyPress(int pcKey, bool pressed, int velocity); 	//pressed = false means released
	void setNote(int note, bool press, int velocity); 	//pressed = false means released
	
	
}
