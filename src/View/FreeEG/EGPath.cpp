#include "EGPath.h"
#include <QPainter>

EGPath::EGPath(const QColor& color) : bounds(0, 0, 1148, 256), color(color)
{
	for (int i = 0; i < 192; i++)
	{
		eg_poly.append(QPointF(i * 6, 128));
	}
}

void EGPath::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	painter->setRenderHint(QPainter::Antialiasing);

	painter->setOpacity(0.5);

	QPen pen; pen.setColor(color); pen.setWidth(5);
	pen.setCosmetic(true);
	pen.setJoinStyle(Qt::PenJoinStyle::RoundJoin);
	painter->setPen(pen);

	QPainterPath path1; path1.addPolygon(eg_poly);
	painter->drawPath(path1);
}


QRectF EGPath::boundingRect() const
{
    return bounds;
}

void EGPath::setPoint(int index, int value)
{
	eg_poly[index].setY((value-128)*-1);
	update();
}

void EGPath::setTrack(const std::array<int, 192>& track)
{
    for (size_t i = 0; i < track.size(); i++) setPoint(i, track[i]);
}
