#include "FreeEGScene.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>


FreeEGScene::FreeEGScene(QObject *parent)
	: QGraphicsScene(parent)
{

	int pos = 0;

	for (auto& r : rects)
	{
		r = new QGraphicsRectItem();
		r->setRect(0, 0, 6, 256);
		r->setX(pos);
		r->setPen(QPen(Qt::lightGray));
		this->addItem(r);
		pos += 6;
	}
	
	QColor colors[4]{ Qt::darkBlue, Qt::green, Qt::red, Qt::darkYellow };

	for (int i = 3; i > -1; i--)
	{
		path[i] = new EGPath(colors[i]);
		addItem(path[i]);
		path[i]->setX(3);
		path[i]->setTrack(track[i]);
	}

	QGraphicsLineItem* middle = new QGraphicsLineItem(0, 128, pos, 128);
	middle->setPen(QPen(Qt::gray));
	addItem(middle);

}

std::vector<int> FreeEGScene::getTrackData()
{
	std::vector<int> result;


	auto lambda = [&](const EGTrack& track, std::vector<int>& result) {


		for (auto t : track)
		{
			result.push_back(t < 0 ? 0 : 1);
			if (t < 0) t += 128;
			result.push_back(t);
		}

	};

	lambda(track[0], result);
	lambda(track[1], result);
	lambda(track[2], result);
	lambda(track[3], result);

	return result;
}

void FreeEGScene::setTrackData(const std::vector<int>& trackData)
{
	//if (trackData.size() != 192 * 4) return;

	for (int i = 0; i < trackData.size(); i++)
	{
		int currentTrackNo = i / 192;
		auto& currentTrack = track[currentTrackNo];
		currentTrack[i % 192] = trackData[i];
	}

	for (int i = 0; i < 4; i++) {
		path[i]->setTrack(track[i]);
	}
}


void FreeEGScene::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
	if (e->buttons() == Qt::LeftButton) processPosition(e->scenePos());
}

void FreeEGScene::setCurrentIndex(int index)
{
	current_path = path[index];
	current_track = &track[index];
}

void FreeEGScene::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
	if (e->buttons() == Qt::LeftButton) processPosition(e->scenePos());

	if (e->buttons() == Qt::RightButton) {
		for (auto& p : *current_track) p = 0;

		current_path->setTrack(*current_track);
		emit editingFinished();
	}
}

void FreeEGScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
	if (e->button() == Qt::LeftButton) emit editingFinished();
}

void FreeEGScene::processPosition(QPointF pos)
{
	int x = static_cast<int>(pos.x()) / 6;
	int y = (static_cast<int>(pos.y()) * -1) + 128;

	if (x < 0 || x > 191) return;
	if (y < -128 || y > 128) return;

	current_path->setPoint(x, y);

	current_track->at(x) = y;
}



FreeEGScene::~FreeEGScene()
{}
