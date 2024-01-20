#include "DbTableView.h"
#include <qdebug.h>
#include <QDropEvent>
#include <QHeaderView>
#include <QKeyEvent>

DbTableView::DbTableView(QWidget* parent) : QTableView(parent)
{

	//setSelectionMode(QAbstractItemView::ExtendedSelection);
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
