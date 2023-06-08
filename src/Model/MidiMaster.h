#pragma once

#include "qmidiout.h"
#include "qmidiin.h"
#include "An1x.h"
#include <optional>
#include <variant>

class QAN1xEditor;

typedef std::vector<unsigned char> Message;

class MidiMaster
{
	QMidiOut* m_out { nullptr };
	QMidiIn* m_in {nullptr};

	QAN1xEditor* view{ nullptr };

	bool handlingMessage = false;
	bool sendingMessage = false;

	bool handleSceneParameter(const Message& m);
	bool handleCommonParameter(const Message& m);
	bool handleSequenceParameter(const Message& m);
	bool handleGlobalParameter(const Message& m);

	void sendMessage(const Message& msg);
	void handleMessage(const Message& msg);

	MidiMaster();

	static MidiMaster m_singleton;

public:

	static MidiMaster& get() { return m_singleton; }

	void setView(QAN1xEditor* v) { view = v; }

	void refreshConnection();

	void connectMidiIn(int idx);
	void connectMidiOut(int idx);

	void setParam(AN1x::ParamType type, unsigned char parameter, int value);
};
