#include "qmidiin.h"
#include <QDebug>
QMidiIn::QMidiIn(QObject *parent) : QObject(parent),
    _midiIn(new RtMidiIn())
{
    _midiIn->setCallback(&QMidiIn::callback, this);
}

QStringList QMidiIn::getPorts()
{
    //TODO: make this static
    QStringList ports;
    for(unsigned int i = 0; i < _midiIn->getPortCount(); i++)
    {
        ports.append(QString::fromStdString(_midiIn->getPortName(i)));
    }
    return ports;
}
void QMidiIn::openPort(unsigned int index)
{
    _midiIn->openPort(index);
}

void QMidiIn::openVirtualPort(QString name)
{
    _midiIn->openVirtualPort(name.toStdString());
}
void QMidiIn::openPort(QString name)
{
    for(unsigned int i = 0; i < _midiIn->getPortCount(); i++)
    {
        if(name == QString::fromStdString(_midiIn->getPortName(i)))
        {
            _midiIn->openPort(i);
            return;
        }
    }
}
void QMidiIn::closePort()
{
    _midiIn->closePort();
}
bool QMidiIn::isPortOpen()
{
    return _midiIn->isPortOpen();
}
void QMidiIn::setIgnoreTypes(bool sysex, bool time, bool sense)
{
    _midiIn->ignoreTypes(sysex, time, sense);
}
void QMidiIn::onMidiMessageReceive(QMidiMessage *msg)
{
    msg->moveToThread(thread());
    emit midiMessageReceived(msg);
}

void QMidiIn::callback(double deltatime, std::vector<unsigned char> *message, void *userData)
{
    QMidiIn* midiIn = (QMidiIn*) userData;
    QMidiMessage *midiMessage = new QMidiMessage();

        if((message->at(0)) >= MIDI_SYSEX) {
            midiMessage->setStatus((QMidiStatus)(message->at(0) & 0xFF));
            midiMessage->setChannel(0);
        } else {
            midiMessage->setStatus((QMidiStatus) (message->at(0) & 0xF0));
            midiMessage->setChannel((message->at(0) & 0x0F)+1);
        }

        midiMessage->setDeltaTime(deltatime*1000); //convert s to ms
//        midiMessage->portNum = portNum;
//        midiMessage->portName = portName;

        switch(midiMessage->getStatus()) {
            case MIDI_NOTE_ON :
            case MIDI_NOTE_OFF:
                midiMessage->setPitch((unsigned int) message->at(1));
                midiMessage->setVelocity((unsigned int) message->at(2));
                break;
            case MIDI_CONTROL_CHANGE:
                midiMessage->setControl((unsigned int) message->at(1));
                midiMessage->setValue((unsigned int) message->at(2));
                break;
            case MIDI_PROGRAM_CHANGE:
            case MIDI_AFTERTOUCH:
                midiMessage->setControl((unsigned int) message->at(1));
                break;
            case MIDI_PITCH_BEND:
                midiMessage->setValue((unsigned int) (message->at(2) << 7) +
                                    (unsigned int) message->at(1)); // msb + lsb
                break;
            case MIDI_POLY_AFTERTOUCH:
                midiMessage->setPitch((unsigned int) message->at(1));
                midiMessage->setValue((unsigned int) message->at(2));
                break;
            case MIDI_SYSEX:
                midiMessage->setSysExData(*message);
                break;
            default:
                break;
        }

    unsigned int nBytes = message->size();
    qDebug()<<"channel"<<(int)(message->at(0))-144;
    for ( unsigned int i=0; i<nBytes; i++ )
      std::cout << "Byte " << i << " = " << (int)message->at(i) << ", ";
    if ( nBytes > 0 )
      std::cout << "stamp = " << deltatime << std::endl;

    midiIn->onMidiMessageReceive(midiMessage);

}

