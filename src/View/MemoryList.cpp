#include "MemoryList.h"
#include <QDropEvent>

MemoryList::MemoryList(QWidget* parent) : QListWidget(parent)
{

}

void MemoryList::dropEvent(QDropEvent* e)
{
	if (e->source() == this) {
		QListWidget::dropEvent(e);
		return;
	}

	emit dataDroped(row(itemAt(e->pos())));
}


MemoryList::~MemoryList()
{
}
