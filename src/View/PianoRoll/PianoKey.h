#pragma once

#include <QGraphicsRectItem>

class PianoKey  : public QGraphicsRectItem
{
	int idx;
	bool m_black;

	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
	void hoverEnterEvent(QGraphicsSceneHoverEvent* e) override;
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

public:
	PianoKey(int idx);
	bool isBlack() { return m_black; };
	bool isWhite() { return !m_black; };

	void noteOn();
	void noteOff();

	~PianoKey();
};
