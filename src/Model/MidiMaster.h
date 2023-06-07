#pragma once

#include "qmidiout.h"
#include "qmidiin.h"
#include "An1x.h"
#include <optional>

class QAN1xEditor;

typedef std::vector<unsigned char> Message;

class MidiMaster
{
	QMidiOut* m_out { nullptr };
	QMidiIn* m_in {nullptr};

	QAN1xEditor* view{ nullptr };

	bool handlingMessage = false;

	bool handleSceneParameter(const Message& m);
	bool handleCommonParameter(const Message& m);

	void sendMessage(const Message& msg);
	void handleMessage(const Message& msg);

public:

	MidiMaster();

	void setView(QAN1xEditor* v) { view = v; }

	void refreshConnection();

	void connectMidiIn(int idx);
	void connectMidiOut(int idx);
	
	void setCommonParam(AN1x::CommonParam p, int value);
	void setSceneParam(AN1x::SceneParam p, int value, bool isScene2);
	
};
