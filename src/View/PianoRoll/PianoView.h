#pragma once

#include <QGraphicsView>
#include "PianoKey.h"

class PianoView  : public QGraphicsView
{
	Q_OBJECT

	std::array<PianoKey*, 128> p_keys{ nullptr };
	int m_velocity{ 64 };
public:
	PianoView(QWidget *parent);
	void setNote(int note, bool pressed);
	void setVelocity(int velocity);
	~PianoView();
};
