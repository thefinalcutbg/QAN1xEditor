#pragma once
#include <QGraphicsRectItem>

class EGRect : public QGraphicsRectItem
{

	int m_index{ -1 };
	bool m_underMouse = false;
	void mousePressEvent(QGraphicsSceneMouseEvent* e) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* e) override;
	void hoverEnterEvent(QGraphicsSceneHoverEvent* e) override;
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* e) override;
public:
	EGRect(int index);
};