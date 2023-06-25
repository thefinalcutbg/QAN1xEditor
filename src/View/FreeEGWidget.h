#pragma once

#include <QWidget>
#include "ui_FreeEGWidget.h"

class FreeEGWidget : public QWidget
{
	Q_OBJECT

public:
	FreeEGWidget(QWidget *parent = nullptr);
	~FreeEGWidget();

private:
	Ui::FreeEGWidgetClass ui;
};
