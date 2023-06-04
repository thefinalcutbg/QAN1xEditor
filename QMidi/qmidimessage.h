#ifndef QMIDIMESSAGE_H
#define QMIDIMESSAGE_H

#include <QObject>
#include <vector>

//https://github.com/danomatika/ofxMidi/blob/master/src/ofxMidiConstants.h
enum QMidiStatus {

    MIDI_UNKNOWN            = 0x00,

    // channel voice messages
    MIDI_NOTE_OFF           = 0x80,
    MIDI_NOTE_ON            = 0x90,
    MIDI_CONTROL_CHANGE     = 0xB0,
    MIDI_PROGRAM_CHANGE     = 0xC0,
    MIDI_PITCH_BEND         = 0xE0,
    MIDI_AFTERTOUCH         = 0xD0, // aka channel pressure
    MIDI_POLY_AFTERTOUCH    = 0xA0, // aka key pressure

    // system messages
    MIDI_SYSEX              = 0xF0,
    MIDI_TIME_CODE          = 0xF1,
    MIDI_SONG_POS_POINTER   = 0xF2,
    MIDI_SONG_SELECT        = 0xF3,
    MIDI_TUNE_REQUEST       = 0xF6,
    MIDI_SYSEX_END          = 0xF7,
    MIDI_TIME_CLOCK         = 0xF8,
    MIDI_START              = 0xFA,
    MIDI_CONTINUE           = 0xFB,
    MIDI_STOP               = 0xFC,
    MIDI_ACTIVE_SENSING     = 0xFE,
    MIDI_SYSTEM_RESET       = 0xFF
};

// number range defines
// because it's sometimes hard to remember these  ...
#define MIDI_MIN_BEND       0
#define MIDI_MAX_BEND       16383

class QMidiMessage : public QObject
{
    Q_OBJECT
public:
    explicit QMidiMessage(QObject *parent = 0);
    ~QMidiMessage();
    QMidiMessage(const QMidiMessage &other);

    void clear();
    QMidiStatus getStatus();
    unsigned int getChannel();
    unsigned int getPitch();
    unsigned int getVelocity();
    unsigned int getControl();
    unsigned int getValue();
    double getDeltaTime();
    std::vector<unsigned char> getSysExData();


    QMidiMessage* setStatus(QMidiStatus status);
    QMidiMessage* setChannel(unsigned int channel);
    QMidiMessage* setPitch(unsigned int pitch);
    QMidiMessage* setVelocity(unsigned int velocity);
    QMidiMessage* setControl(unsigned int control);
    QMidiMessage* setValue(unsigned int value);
    QMidiMessage* setDeltaTime(double deltaTime);
    QMidiMessage* setSysExData(std::vector<unsigned char> sysExData);
    QMidiMessage* setRawMessage(std::vector<unsigned char> rawMessage);

    std::vector<unsigned char> getRawMessage();

public:
    QMidiStatus _status;
    unsigned int _channel;
    unsigned int _pitch;
    unsigned int _velocity;
    unsigned int _control;
    unsigned int _value;
    double _deltaTime;
    std::vector<unsigned char> _sysExData;

    std::vector<unsigned char> _rawMessage;
signals:

public slots:
};

#endif // QMIDIMESSAGE_H
