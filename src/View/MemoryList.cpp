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

void MemoryList::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key_Delete:
		emit deleteRequested();
		return;
	case Qt::Key_C:
		if (event->modifiers() & Qt::ControlModifier) {
			emit copyRequested();
			return;
		}
		break;
		
	case Qt::Key_V:
		if (event->modifiers() & Qt::ControlModifier) {
			emit pasteRequested();
			return;
		}
		break;
	}

	QListWidget::keyPressEvent(event);
}


MemoryList::~MemoryList()
{
}
