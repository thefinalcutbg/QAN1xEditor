#pragma once

#include <QGraphicsItem>
#include <array>

class EGPath : public QGraphicsItem
{

	QPolygonF eg_poly;

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

	QRectF bounds;

	QColor color;

public:
	EGPath(const QColor& color);

	QRectF boundingRect() const override;

	void setPoint(int index, int value);
	void setTrack(const std::array<int, 192>& track);
};