#include "PianoKey.h"
#include <QBrush>
#include <QPen>
#include "Model/MidiMaster.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <qdebug.h>

void PianoKey::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{

}

void PianoKey::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        MidiMaster::setNote(idx, true);
    }
}

void PianoKey::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        MidiMaster::setNote(idx, false);
    }
}

void PianoKey::hoverEnterEvent(QGraphicsSceneHoverEvent* e)
{
    
}

void PianoKey::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{

    MidiMaster::setNote(idx, false);
}

PianoKey::PianoKey(int idx) : idx(idx)
{
    setAcceptHoverEvents(true);

    int position = idx % 12;

    if (position == 0 || position == 2 || position == 4 || position == 5 || position == 7 || position == 9 || position == 11)
    {
        m_black = false;
    }
    else
    {
        m_black = true;
    };

   QPen pen;
   pen.setColor(Qt::black);

   setPen(pen);
   setBrush(m_black ? Qt::black : Qt::white);

   setRect(m_black ?
       QRect(0, 0, 10, 30)
       :
       QRect(0, 0, 15, 50)
   );
}


void PianoKey::noteOn()
{
    setBrush(Qt::gray);
}

void PianoKey::noteOff()
{
    setBrush(m_black ? Qt::black : Qt::white);
}

PianoKey::~PianoKey()
{}
