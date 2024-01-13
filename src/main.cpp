#include <QApplication>
#include <QDebug>
#include "View/QAN1xEditor.h"
#include "RtMidi.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	a.setWheelScrollLines(1);

	QAN1xEditor w;
	w.show();

	return a.exec();

}