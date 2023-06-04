#pragma once

#include <QDialog>
#include "ui_ValueDialog.h"

class ValueDialog : public QDialog
{
	Q_OBJECT

	int m_current{ -1 };
	int m_default{ -1 };
	int m_result{ -1 };

public:
	ValueDialog(QWidget *parent = nullptr);
	void setValue(int min, int max, int dafaultValue, int currentValue);
	int getValue();
	~ValueDialog();

private:
	Ui::ValueDialogClass ui;
};
