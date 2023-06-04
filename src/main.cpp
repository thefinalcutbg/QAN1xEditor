#include <QApplication>
#include <QDebug>
#include "QMidiAn1x.h"


int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	a.setWheelScrollLines(1);

	QMidiAn1x w;
	w.show();

	return a.exec();

}