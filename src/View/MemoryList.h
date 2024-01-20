#pragma once

#include <QListWidget>

class MemoryList : public QListWidget
{
	Q_OBJECT

public:
	MemoryList(QWidget* parent);


	~MemoryList();

signals:
	void dataDroped(int row);

private:
	void dropEvent(QDropEvent* e) override;

};