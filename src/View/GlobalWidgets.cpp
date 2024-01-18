#include "GlobalWidgets.h"
#include <QMessageBox>

bool GlobalWidgets::askQuestion(const char* question)
{
	QMessageBox::StandardButton reply;

	reply = QMessageBox::question(nullptr, "QAN1xEditor", question,
		QMessageBox::Yes | QMessageBox::No);

	return reply != QMessageBox::No;
}
