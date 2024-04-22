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

    int m_currentIndex {0};

    bool m_resetToZero {false};

	EGPath* path[4];
	EGTrack track[4]{ {0},{0},{0},{0} };
    EGTrack default_track[4]{ {0},{0},{0},{0} };

    EGPath& currentPath()   { return *path[m_currentIndex]; }
    EGTrack& currentTrack() { return track[m_currentIndex]; }
    EGTrack& defaultTrack() { return default_track[m_currentIndex]; }


public:

    FreeEGScene(QObject *parent);

    void setCurrentIndex(int index);
    std::vector<int> getFreeEGData();
    void setResetMode(bool resetToZero) { m_resetToZero = resetToZero; };
	void setTrackData(const std::vector<int>& trackData);
    void quickAssign(int from, int to, int value);

    ~FreeEGScene();


signals:
	void editingFinished();
};
