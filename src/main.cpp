#include <QApplication>
#include "View/QAN1xEditor.h"
#include "Database/Database.h"
#include "View/FreeFunctions.h"

int main(int argc, char* argv[])
{
	Db::createIfNotExist();

	QApplication a(argc, argv);

	a.setWindowIcon(QIcon(":/icon/resources/appIcon.png"));

	if (FreeFn::getUpdate()) return 0;

	a.setWheelScrollLines(1);

	QAN1xEditor w;
    w.setWindowTitle("QAN1xEditor v1.2");
	w.show();

	return a.exec();

}
