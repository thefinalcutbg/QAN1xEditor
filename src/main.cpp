#include <QApplication>
#include <QDebug>
#include "View/QAN1xEditor.h"
#include "Database/Database.h"

int main(int argc, char* argv[])
{
	Db::createIfNotExist();

	QApplication a(argc, argv);

	a.setWheelScrollLines(1);

	QAN1xEditor w;
	w.show();

	return a.exec();

}