#include "PianoKey.h"
#include <QBrush>
#include <QPen>
#include "Model/MidiMaster.h"
#include <QGraphicsSceneMouseEvent>

void PianoKey::mouseMoveEvent(QGraphicsSceneMouseEvent*)
{

}

void PianoKey::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        MidiMaster::setNote(idx, true, m_velocity);
    }
}

void PianoKey::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        MidiMaster::setNote(idx, false, m_velocity);
    }
}

void PianoKey::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{

}

void PianoKey::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
    MidiMaster::setNote(idx, false, m_velocity);
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
    update();
}

void PianoKey::noteOff()
{
    if (m_black) {
        setBrush(Qt::black);
        return;
    }

    m_highlighted ? setBrush(QColor(214, 220, 241)) : setBrush(Qt::white);

    update();
}

void PianoKey::highlight(bool highlighted)
{
    m_highlighted = highlighted;

    if (m_black) return;

    m_highlighted ? setBrush(QColor(214, 220, 241)) : setBrush(Qt::white);

    update();
}

PianoKey::~PianoKey()
{}
