#include "MidiMaster.h"
#include "qmidimessage.h"
#include <QDebug>
#include "View/QAN1xEditor.h"
#include <array>


MidiMaster MidiMaster::m_singleton;

MidiMaster::MidiMaster()
{

}


void MidiMaster::refreshConnection()
{
	if (m_out) {
		m_out->closePort();
		delete m_out;
	}

	if (m_in)
	{
		m_in->closePort();
		delete m_in;
	}
	
	m_out = new QMidiOut;
	m_in = new QMidiIn;

	m_in->setIgnoreTypes(false, true, true);

	QObject::connect(m_in, &QMidiIn::midiMessageReceived, [=](QMidiMessage* m) {  handleMessage(m->getSysExData()); delete m; });

	view->setMidiDevices(m_in->getPorts(), m_out->getPorts());
	
}

void MidiMaster::connectMidiIn(int idx)
{
	if (!m_in) return;

	if (idx == -1) return; 

	m_in->openPort(idx);
}

void MidiMaster::connectMidiOut(int idx)
{
	if (!m_out) return;

	if (idx == -1) return;
		
	m_out->openPort(idx);
}

bool MidiMaster::handleCommonParameter(const Message& m)
{

	static const std::array<int, 6> header = { 240, 67, 16, 92, 16, 0 };

	bool isCommonParam{ true };

	for (int i = 0; i < header.size(); i++)
	{
		if (i >= m.size() || header[i] != m[i]) {
			isCommonParam = false;
			break;
		}
	}

	if (!isCommonParam) return false;

	AN1x::CommonParam parameter = static_cast<AN1x::CommonParam>(m[6]);

	if (AN1x::isNull(AN1x::ParamType::Common, parameter)) return false;

	int value = m[7];

	if (AN1x::isTwoByteParameter(AN1x::ParamType::Common, parameter))
	{
		value = m[7]*128;
		value += m[8];
	}

	value -= AN1x::getOffset(AN1x::ParamType::Common, parameter);

	if (!view) return true;

	view->setCommonParameter(parameter, value);

	return true;
}

bool MidiMaster::handleSceneParameter(const Message& m)
{

	static const std::array<int, 5> header = { 240, 67, 16, 92, 16 };


	for (int i = 0; i < header.size(); i++)
	{
		if (i >= m.size() || header[i] != m[i]) return false;
	}


	if (m[5] != 16 && m[5] != 17) return false;

	bool isScene2 = m[5] == 16 ? false : true;
	AN1x::SceneParam parameter = static_cast<AN1x::SceneParam>(m[6]);

	if (AN1x::isNull(AN1x::ParamType::Scene1, parameter)) return false;

	int value = m[7];

	if (AN1x::isTwoByteParameter(AN1x::ParamType::Scene1, parameter))
	{
		value = m[7] * 128;
		value += m[8];
	}

	value -= AN1x::getOffset(AN1x::ParamType::Scene1, parameter);

	if (!view) return true;

	view->setSceneParameter(parameter, value, isScene2);

	return true;

}

bool MidiMaster::handleSequenceParameter(const Message& m)
{
	static const std::array<int, 6> header = { 240, 67, 16, 92, 16, 14 };

	for (int i = 0; i < header.size(); i++)
	{
		if (i >= m.size() || header[i] != m[i]) return false;
	}


	view->setSequenceParameter((AN1x::SeqParam)m[6], m[7]);

	return true;
}

bool MidiMaster::handleGlobalParameter(const Message& m)
{
	return false;
}


void MidiMaster::setParam(AN1x::ParamType type, unsigned char parameter, int value)
{
	auto header = AN1x::getHeader(type);

	header.push_back(parameter);

	if (value < 128) {
		header.push_back(value);
	}
	else
	{
		header.push_back(value / 128);
		header.push_back(value % 128);
	}

	header.push_back(0xF7);
}


void MidiMaster::handleMessage(const Message& msg)
{
	if(sendingMessage) return;
	qDebug() << "recieve: "<< msg;
	handlingMessage = true;

	handleCommonParameter(msg);
	handleSceneParameter(msg);
	handleSequenceParameter(msg);

	handlingMessage = false;
}



void MidiMaster::sendMessage(const Message& msg)
{
	if (handlingMessage) return;

	sendingMessage = true;

	try {
		if (!m_out->isPortOpen()) return;

		auto m = msg;

		qDebug() << "send: " << m;

		m_out->sendRawMessage(m);
	}
	catch (std::exception)
	{
		refreshConnection();
	}

	sendingMessage = false;
}



