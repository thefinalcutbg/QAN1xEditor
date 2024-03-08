#pragma once

#include <QGraphicsRectItem>

class PianoKey  : public QGraphicsRectItem
{
	int idx;
	bool m_black;

	bool m_highlighted{ false };
	bool m_pressed{ false };

	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

	int m_velocity{ 64 };

public:
	PianoKey(int idx);
	bool isBlack() { return m_black; };
	bool isWhite() { return !m_black; };

	void noteOn();
	void noteOff();

	void highlight(bool highlighted);

	void setVelocity(int velocity) { m_velocity = velocity; }

	~PianoKey();
};
