#include "MidiMaster.h"
#include "qmidimessage.h"
#include <QDebug>
#include "QMidiAn1x.h"
#include <array>

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

	if (AN1x::isNull(parameter)) return false;

	int value = m[7];

	if (AN1x::isTwoByteParameter(parameter))
	{
		value = m[7]*128;
		value += m[8];
	}

	value -= AN1x::getOffset(parameter);

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

	if (AN1x::isNull(parameter)) return false;

	int value = m[7];

	if (AN1x::isTwoByteParameter(parameter))
	{
		value = m[7] * 128;
		value += m[8];
	}

	value -= AN1x::getOffset(parameter);

	if (!view) return true;

	view->setSceneParameter(parameter, value, isScene2);

	return true;

}


void MidiMaster::handleMessage(const Message& msg)
{
	qDebug() << msg;
	handlingMessage = true;

	handleCommonParameter(msg);
	handleSceneParameter(msg);

	handlingMessage = false;
}


void MidiMaster::sendMessage(const Message& msg)
{
	if (handlingMessage) return;

	try {
		if (!m_out->isPortOpen()) return;

		auto m = msg;

		m_out->sendRawMessage(m);
	}
	catch (std::exception)
	{
		refreshConnection();
	}
}

//0xF0, 0x43, 0x10, 0x5C, 0x10, 0x00

void MidiMaster::setCommonParam(AN1x::CommonParam p, int value)
{
	if (AN1x::isNull(p)) return;

	value += getOffset(p);

	if (AN1x::isTwoByteParameter(p))
	{
		unsigned char firstByte = value / 128;
		unsigned char secondByte = value % 128;

		sendMessage({ 0xF0, 0x43, 0x10, 0x5C, 0x10, 0x00, unsigned char(p), firstByte, secondByte, 0xF7 });

		return;
	}

	sendMessage({ 0xF0, 0x43, 0x10, 0x5C, 0x10, 0x00, unsigned char(p), unsigned char(value), 0xF7 });
}

//0xF0, 0x43, 0x10, 0x5C, 0x10,
void MidiMaster::setSceneParam(AN1x::SceneParam p, int value, bool isScene2)
{

	if (AN1x::isNull(p)) return;

	value += getOffset(p);

	if (AN1x::isTwoByteParameter(p))
	{
		unsigned char firstByte = value/128;
		unsigned char secondByte = value%128;

		sendMessage({ 0xF0, 0x43, 0x10, 0x5C, 0x10, AN1x::getScene(isScene2), p, firstByte, secondByte, 0xF7 });

		return;
	}

	sendMessage({ 0xF0, 0x43, 0x10, 0x5C, 0x10, AN1x::getScene(isScene2), p, (unsigned char)value, 0xF7 });
}

