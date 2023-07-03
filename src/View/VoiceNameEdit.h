#pragma once

#include <QLineEdit>
#include "Model/An1x.h"

class VoiceNameEdit  : public QLineEdit
{
	Q_OBJECT

	QString default_name = "InitNormal";

public:
	VoiceNameEdit(QWidget *parent);
	void setName(AN1x::CommonParam p, int value);
	unsigned char getChar(int index);
	~VoiceNameEdit();
};
