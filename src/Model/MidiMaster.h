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
	void setPress(int note);
	void setRelease(int note);
}
