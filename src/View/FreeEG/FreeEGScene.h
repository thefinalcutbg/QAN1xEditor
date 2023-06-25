#pragma once

#include <QGraphicsScene>

class FreeEGScene  : public QGraphicsScene
{
	Q_OBJECT

public:
	FreeEGScene(QObject *parent);
	~FreeEGScene();
};
