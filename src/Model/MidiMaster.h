#pragma once


#include "An1x.h"
#include <vector>

class QAN1xEditor;

typedef std::vector<unsigned char> Message;

namespace MidiMaster
{

	void setView(QAN1xEditor* v);

	void refreshConnection();

	void connectMidiIn(int idx);
	void connectMidiOut(int idx);

	void setParam(AN1x::ParamType type, unsigned char parameter, int value);

	void modWheelChange(int value);
	void pitchChange(int value);
	void goToVoice(int value);
	void syncBulk(const Message& = {});

	void sendCommonBulk();

	//PC keyboard as Midi

	void setKbdOctave(int octave);

	void pcKeyPress(int pcKey, bool pressed, int velocity); 	//pressed = false means released
	void setNote(int note, bool press, int velocity); 	//pressed = false means released

	
}
