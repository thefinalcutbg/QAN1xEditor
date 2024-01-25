#include "DbTableView.h"

#include <QDropEvent>
#include <QHeaderView>
#include <QKeyEvent>
#include <QStyledItemDelegate>

class NoFocusDelegate : public QStyledItemDelegate
{
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyleOptionViewItem itemOption(option);
		if (itemOption.state & QStyle::State_HasFocus)
			itemOption.state = itemOption.state ^ QStyle::State_HasFocus;
		QStyledItemDelegate::paint(painter, itemOption, index);
	}

};

DbTableView::DbTableView(QWidget* parent) : QTableView(parent)
{

	setItemDelegate(new NoFocusDelegate);
		setSelectionBehavior(QAbstractItemView::SelectRows);

	horizontalHeader()->setHighlightSections(false);

	verticalHeader()->setVisible(false);

	setEditTriggers(QAbstractItemView::EditTrigger::NoEditTriggers);
	viewport()->setFocusPolicy(Qt::FocusPolicy::StrongFocus);

	setShowGrid(true);

	verticalHeader()->setDefaultSectionSize(20);
	setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	horizontalHeader()->setStretchLastSection(true);

	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void DbTableView::dropEvent(QDropEvent* e)
{
	if (e->source() == this) return;

	emit dataDroped();
}

void DbTableView::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key_Delete:
		emit deletePressed();
		break;
	case Qt::Key_C:
		if (event->modifiers() & Qt::ControlModifier) {
			emit copyRequested();
		}
		else {
			QTableView::keyPressEvent(event);
		}
		
	default:
		QTableView::keyPressEvent(event);
	}
}

DbTableView::~DbTableView()
{
}
