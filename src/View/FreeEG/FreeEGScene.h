#pragma once

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include "EGPath.h"
#include <vector>

typedef std::array<int, 192> EGTrack;

class FreeEGScene  : public QGraphicsScene
{
	Q_OBJECT

	QGraphicsRectItem* rects[192];

	void mouseMoveEvent(QGraphicsSceneMouseEvent* e) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* e) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* e) override;
	void processPosition(QPointF pos, bool reset);

	EGPath* current_path{ nullptr };
	EGTrack* current_track{ nullptr };

	EGPath* path[4];

	EGTrack track[4]{ {0},{0},{0},{0} };

public:
	
	void setCurrentIndex(int index);
	FreeEGScene(QObject *parent);
	std::vector<int> getTrackData();
	~FreeEGScene();


signals:
	void editingFinished();
};
