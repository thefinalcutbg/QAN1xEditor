#pragma once

#include <QGraphicsView>
#include "PianoKey.h"

class PianoView  : public QGraphicsView
{
	Q_OBJECT

	std::array<PianoKey*, 128> p_keys{ nullptr };

public:
	PianoView(QWidget *parent);
	void setNote(int note, bool pressed);

	~PianoView();
};
