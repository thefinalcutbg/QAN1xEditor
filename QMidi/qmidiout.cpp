#include "qmidiout.h"
#include <QDebug>
QMidiOut::QMidiOut(QObject *parent) : QObject(parent),
    _midiOut(new RtMidiOut())
{

}
QStringList QMidiOut::getPorts()
{
    //TODO: make this static
    QStringList ports;
    for(unsigned int i = 0; i < _midiOut->getPortCount(); i++)
    {
        ports.append(QString::fromStdString(_midiOut->getPortName(i)));
    }
    return ports;
}
void QMidiOut::openPort(unsigned int index)
{
    _midiOut->openPort(index);
}

void QMidiOut::openVirtualPort(QString name)
{
    _midiOut->openVirtualPort(name.toStdString());
}
void QMidiOut::closePort()
{
    _midiOut->closePort();
}
bool QMidiOut::isPortOpen()
{
    return _midiOut->isPortOpen();
}
void QMidiOut::sendNoteOn(unsigned int channel, unsigned int pitch, unsigned int velocity)
{
    std::vector<unsigned char> message;
    message.push_back(MIDI_NOTE_ON+channel-1);
    message.push_back(pitch);
    message.push_back(velocity);
    sendRawMessage(message);
}
void QMidiOut::sendNoteOff(unsigned int channel, unsigned int pitch, unsigned int velocity)
{
    std::vector<unsigned char> message;
    message.push_back(MIDI_NOTE_OFF+(channel-1));
    message.push_back(pitch);
    message.push_back(velocity);
    sendRawMessage(message);
}
void QMidiOut::sendMessage(QMidiMessage *message)
{
    std::vector<unsigned char> rawMessage = message->getRawMessage();
    if(!rawMessage.empty()){
        qDebug()<<"send message"<<rawMessage.front();
        sendRawMessage(rawMessage);
    }
    
}

void QMidiOut::sendRawMessage(std::vector<unsigned char> &message)
{
    _midiOut->sendMessage(&message);
}


