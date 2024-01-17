#include "GlobalWidgets.h"
#include <QMessageBox>

bool GlobalWidgets::askQuestion(const char* title, const char* question)
{
	QMessageBox::StandardButton reply;

	reply = QMessageBox::question(nullptr, title, question,
		QMessageBox::Yes | QMessageBox::No);

	return reply != QMessageBox::No;
}
