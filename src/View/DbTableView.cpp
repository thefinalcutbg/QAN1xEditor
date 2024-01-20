#include "DbTableView.h"
#include <qdebug.h>
#include <QDropEvent>
#include <QHeaderView>

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

DbTableView::~DbTableView()
{
}
