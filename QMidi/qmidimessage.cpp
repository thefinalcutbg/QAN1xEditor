#include "qmidimessage.h"

QMidiMessage::QMidiMessage(QObject *parent) : QObject(parent)
{
    clear();
}

QMidiMessage::~QMidiMessage()
{

}

QMidiMessage::QMidiMessage(const QMidiMessage &other)
{
    _status = other._status;
    _channel = other._channel;
    _pitch = other._pitch;
    _velocity = other._velocity;
    _control = other._control;
    _value = other._value;
    _deltaTime = other._deltaTime;
    _sysExData = other._sysExData;
    _rawMessage = other._rawMessage;
}

void QMidiMessage::clear()
{
    _status = MIDI_UNKNOWN;
    _channel = 1;
    _pitch = 0;
    _velocity = 0;
    _control = 0;
    _value = 0;
    _deltaTime = 0;
    _sysExData.clear();
    _rawMessage.clear();
}

QMidiStatus QMidiMessage::getStatus()
{
    return _status;
}

unsigned int QMidiMessage::getChannel()
{
    return _channel;
}

unsigned int QMidiMessage::getPitch()
{
    return _pitch;
}

unsigned int QMidiMessage::getVelocity()
{
    return _velocity;
}

unsigned int QMidiMessage::getControl()
{
    return _control;
}

unsigned int QMidiMessage::getValue()
{
    return _value;
}

double QMidiMessage::getDeltaTime()
{
    return _deltaTime;
}

std::vector<unsigned char> QMidiMessage::getSysExData()
{
    return _sysExData;
}

QMidiMessage *QMidiMessage::setStatus(QMidiStatus status)
{
    _status = status;
    return this;
}

QMidiMessage *QMidiMessage::setChannel(unsigned int channel)
{
    _channel = channel;
    return this;
}

QMidiMessage *QMidiMessage::setPitch(unsigned int pitch)
{
    _pitch = pitch;
    return this;
}

QMidiMessage *QMidiMessage::setVelocity(unsigned int velocity)
{
    _velocity = velocity;
    return this;
}

QMidiMessage *QMidiMessage::setControl(unsigned int control)
{
    _control = control;
    return this;
}

QMidiMessage *QMidiMessage::setValue(unsigned int value)
{
    _value = value;
    return this;
}

QMidiMessage *QMidiMessage::setDeltaTime(double deltaTime)
{
    _deltaTime = deltaTime;
    return this;
}

QMidiMessage *QMidiMessage::setRawMessage(std::vector<unsigned char> rawMessage)
{
    _rawMessage = rawMessage;
    return this;
}

QMidiMessage *QMidiMessage::setSysExData(std::vector<unsigned char> sysExData){
    _sysExData = sysExData;
    return this;
}

std::vector<unsigned char> QMidiMessage::getRawMessage()
{
    if(_rawMessage.size() == 0)
    {
        switch(_status)
        {
        case MIDI_NOTE_ON:{
            _rawMessage.push_back(MIDI_NOTE_ON+_channel-1);
            _rawMessage.push_back(_pitch);
            _rawMessage.push_back(_velocity);
            break;
        }
        case MIDI_NOTE_OFF:{
            _rawMessage.push_back(MIDI_NOTE_OFF+_channel-1);
            _rawMessage.push_back(_pitch);
            _rawMessage.push_back(_velocity);
            break;
        }
        case MIDI_CONTROL_CHANGE:{
            _rawMessage.push_back(MIDI_CONTROL_CHANGE+_channel-1);
            _rawMessage.push_back(_control);
            _rawMessage.push_back(_value);
            break;
        }
        case MIDI_PROGRAM_CHANGE:{
            _rawMessage.push_back(MIDI_PROGRAM_CHANGE+_channel-1);
            _rawMessage.push_back(_control);
            break;
        }
        case MIDI_SYSEX:{
            _rawMessage = _sysExData;
            if(_sysExData.back() != MIDI_SYSEX_END) _rawMessage.push_back(MIDI_SYSEX_END);
            break;
        }
         
        case MIDI_AFTERTOUCH:{
            _rawMessage.push_back(MIDI_AFTERTOUCH+_channel-1);
            _rawMessage.push_back(_control);
        }

            //TODO: check protocol and implement other cases

        default:{
            break;
        }
        }
    }
    return _rawMessage;
}

