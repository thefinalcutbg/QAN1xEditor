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
	void copyRequested();
	void pasteRequested();

private:
	void dropEvent(QDropEvent* e) override;
	void keyPressEvent(QKeyEvent* event) override;

};