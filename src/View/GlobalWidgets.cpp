#include "GlobalWidgets.h"
#include <QMessageBox>

bool GlobalWidgets::askQuestion(const char* question)
{
	QMessageBox::StandardButton reply;

	reply = QMessageBox::question(nullptr, "QAN1xEditor", question,
		QMessageBox::Yes | QMessageBox::No);

	return reply != QMessageBox::No;
}

void GlobalWidgets::showMessage(const char* message)
{
	QMessageBox::information(nullptr, "QAN1xEditor", message,QMessageBox::Ok);
	return;
}


int GlobalWidgets::YesNoCancelQuestion(const char* question)
{
	QMessageBox::StandardButton reply;

	reply = QMessageBox::question(nullptr, "QAN1xEditor", question,
		QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

	switch (reply) {
		case QMessageBox::No: return 0;
		case QMessageBox::Yes: return 1;
		case QMessageBox::Cancel: return -1;
	}

	return -1;
}
