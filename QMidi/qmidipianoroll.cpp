#include "qmidipianoroll.h"
#include "qmidimessage.h"
#include <QGraphicsRectItem>
#include <QPainter>
QMidiPianoRoll::QMidiPianoRoll(QWidget *parent) :
    QGraphicsView(parent),
    _scene(new QGraphicsScene()),
    _keyWidth(10),
    _keyHeight(60)
{
    float x = 0;
    for(int i = 0; i < 128; i++)
    {
        int position = i % 12;
        QGraphicsRectItem *rect;
        if(position == 0 || position == 2 || position == 4 || position == 5 || position == 7 || position == 9 || position == 11)
        {
            rect = new QGraphicsRectItem(x,0,_keyWidth,_keyHeight);
            _scene->addItem(rect);
            if(position != 4 && position != 11 )
            {
                x += _keyWidth/2+1;
            }
            else
            {
                x += _keyWidth+1;
            }
        }
        else
        {
            rect = new QGraphicsRectItem(x,0,_keyWidth,_keyHeight/2);
            rect->setZValue(2);
            QBrush brush;
            QPen pen;
            brush.setStyle(Qt::SolidPattern);
            pen.setColor(Qt::black);
            brush.setColor(Qt::black);
            rect->setBrush(brush);
            rect->setPen(pen);
            _scene->addItem(rect);
            x += _keyWidth/2+1;
        }
        _keys.push_back(rect);

    }

    setScene(_scene);
}

bool QMidiPianoRoll::isSemiTone(int pitch)
{
    int position = pitch %12;
    if(position == 0 || position == 2 || position == 4 || position == 5 || position == 7 || position == 9 || position == 11)
    {
        return false;
    }
    else
    {
        return true;
    }
}

const QColor& QMidiPianoRoll::pressedColor() const
{
    return _pressedColor;
}

void QMidiPianoRoll::setPressedColor(const QColor& newPressedColor)
{
    _pressedColor = newPressedColor;
}

//void QMidiPianoRoll::paintEvent(QPaintEvent *event)
//{
////    QPainter painter(this);
//}

void QMidiPianoRoll::onMidiReceive(QMidiMessage *message)
{
    switch(message->getStatus())
    {
    case MIDI_NOTE_ON: {
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(QColor(_pressedColor.red(), _pressedColor.green(), _pressedColor.blue(),
                              message->getVelocity() * 2));
        _keys[message->getPitch()]->setBrush(brush);
        break;
    }
    case MIDI_NOTE_OFF: {
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        QColor color;
        if(isSemiTone(message->getPitch()))
        {
            color = QColor(Qt::black);
        }
        else
        {
             color = QColor(Qt::white);
        }
        brush.setColor(color);
        _keys[message->getPitch()]->setBrush(brush);
        break;
    }
    case MIDI_CONTROL_CHANGE: {

        switch (message->getControl()) {
        case 120:
        case 123:
            for (int i = 0; i < 128; ++i) {
                _keys[i]->setBrush(QBrush(isSemiTone(i) ? Qt::black : Qt::white));
            }
            break;
        }

        break;
    }

    default:
        break;
    }
}
