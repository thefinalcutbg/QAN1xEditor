#include "EGRect.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <qdebug.h>

EGRect::EGRect(int index) : m_index(index)
{
	//setAcceptHoverEvents(true);
}

void EGRect::mousePressEvent(QGraphicsSceneMouseEvent* e)
{

	if (e->button() == Qt::LeftButton)
	{
	//	qDebug() << "index" << m_index << "position:" << e->pos().y();
	}
	e->ignore();
	//QGraphicsRectItem::mousePressEvent(e);
}

void EGRect::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{

	//qDebug() << "index" << m_index << "position:" << e->pos().y();
	
	e->ignore();
	//QGraphicsRectItem::mouseMoveEvent(e);
}

void EGRect::hoverEnterEvent(QGraphicsSceneHoverEvent* e)
{
	
	m_underMouse = true;
	//grabMouse();
	//qDebug() <<  m_index << e->pos().y();

	//QGraphicsRectItem::hoverEnterEvent(e);

}

void EGRect::hoverLeaveEvent(QGraphicsSceneHoverEvent* e)
{
	m_underMouse = false;
	
	//QGraphicsRectItem::hoverLeaveEvent(e);
}
