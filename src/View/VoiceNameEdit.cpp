#include "VoiceNameEdit.h"
#include "Model/MidiMaster.h"

VoiceNameEdit::VoiceNameEdit(QWidget *parent)
	: QLineEdit(parent)
{
	setText("InitNormal");

	connect(this, &QLineEdit::textChanged, 
		[&](const QString& text) {
			auto result = text.toStdString();

            for (size_t i = 0; i < 10; i++)
			{
				MidiMaster::parameterChanged(ParamType::Common, (AN1x::CommonParam)i, i >= result.size() ? ' ' : result[i]);
			}
		}
	);
}


void VoiceNameEdit::setName(AN1x::CommonParam p, int value)
{
	blockSignals(true);
    size_t position = (int)p;

	auto currentText = text().toStdString();

	if (position > currentText.size()) {
		
		while (currentText.size() != position + 1) {
			currentText +=' ';
		}
	}

	currentText[p] = (char)value;

	setText(currentText.c_str());

	blockSignals(false);
}

unsigned char VoiceNameEdit::getChar(int index)
{
	auto result = text();

	while (result.length() < 10) result.append(" ");
	
	return result.toStdString()[index];
}

VoiceNameEdit::~VoiceNameEdit()
{}
