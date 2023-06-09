#include "VoiceNameEdit.h"
#include "Model/MidiMaster.h"

VoiceNameEdit::VoiceNameEdit(QWidget *parent)
	: QLineEdit(parent)
{

	connect(this, &QLineEdit::textChanged, 
		[&](const QString& text) {
			auto result = text.toStdString();

			for (int i = 0; i < 10; i++)
			{
				MidiMaster::setParam(AN1x::ParamType::Common, (AN1x::CommonParam)i, i >= result.size() ? ' ' : result[i]);
			}
		}
	);
}


void VoiceNameEdit::setName(AN1x::CommonParam p, int value)
{
	blockSignals(true);
	int position = (int)p;

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

VoiceNameEdit::~VoiceNameEdit()
{}
