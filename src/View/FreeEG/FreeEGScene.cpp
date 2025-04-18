#include "FreeEGScene.h"
#include <QGraphicsSceneMouseEvent>

FreeEGScene::FreeEGScene(QObject *parent)
	: QGraphicsScene(parent)
{

	int pos = 0;

	int counter = 0;

	bool stripeState{ false };

	for (auto& r : rects)
	{

		if (counter % 24 == 0) {
			stripeState = !stripeState;
		}

		r = new QGraphicsRectItem();
		r->setRect(0, 0, 6, 256);
		r->setX(pos);
		r->setPen(QPen(Qt::lightGray));
		r->setBrush(stripeState ? QColor(255,255,255) : QColor(245,245,245));
		this->addItem(r);
		pos += 6;

		counter++;
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

std::vector<int> FreeEGScene::getFreeEGData()
{
	std::vector<int> result;


	auto lambda = [&](const EGTrack& tracks) {


		for (auto t : tracks)
		{
			result.push_back(t < 0 ? 0 : 1);
			if (t < 0) t += 128;
			result.push_back(t);
		}

	};

	lambda(track[0]);
	lambda(track[1]);
	lambda(track[2]);
	lambda(track[3]);

	return result;
}

void FreeEGScene::setTrackData(const std::vector<int>& trackData)
{
	//if (trackData.size() != 192 * 4) return;

    for (size_t i = 0; i < trackData.size(); i++)
	{
		int currentTrackNo = i / 192;

        track[currentTrackNo][i % 192] = trackData[i];
        default_track[currentTrackNo][i % 192] = trackData[i];
	}

	for (int i = 0; i < 4; i++) {
		path[i]->setTrack(track[i]);
	}
}

void FreeEGScene::quickAssign(int from, int to, int value)
{
	if (from > to) {
		std::swap(from, to);
	}

	for (int i = from; i <= to; i++) {
		currentPath().setPoint(i, value);
		currentTrack().at(i) = value;
	}
	emit editingFinished();
}


void FreeEGScene::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
	if (e->buttons() == Qt::LeftButton) processPosition(e->scenePos(), false);
	if (e->buttons() == Qt::RightButton) processPosition(e->scenePos(), true);
}

void FreeEGScene::setCurrentIndex(int index)
{
    if(index < 0 || index > 3) return;

    m_currentIndex = index;
}

void FreeEGScene::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
	if (e->buttons() == Qt::LeftButton) processPosition(e->scenePos(), false);

	if (e->buttons() == Qt::RightButton) processPosition(e->scenePos(), true);

}

void FreeEGScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
	if (e->button() == Qt::LeftButton) emit editingFinished();
	if (e->button() == Qt::RightButton) emit editingFinished();
}

void FreeEGScene::processPosition(QPointF pos, bool reset)
{
	int x = static_cast<int>(pos.x()) / 6;
    if (x < 0 || x > 191) return;

    int y = 0;

    if(!reset){
        y = (static_cast<int>(pos.y()) * -1) + 128;
        y = std::min(128, y);
        y = std::max(-128, y);
    }
    else if(!m_resetToZero){

        y = defaultTrack().at(x);
    }

    currentPath().setPoint(x, y);

    currentTrack().at(x) = y;
}



FreeEGScene::~FreeEGScene()
{}
