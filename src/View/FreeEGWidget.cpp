#include "FreeEGWidget.h"
#include "Model/MidiMaster.h"

FreeEGWidget::FreeEGWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);


	ui_controls = {
		ui.triggerCombo,
		ui.loopCombo,
		ui.lengthCombo,
		ui.keyTrack,
		ui.trackParam_1,
		ui.trackSceneSw_1,
		ui.trackParam_2,
		ui.trackSceneSw_2,
		ui.trackParam_3,
		ui.trackSceneSw_3,
		ui.trackParam_4,
		ui.trackSceneSw_4
	};

	ComboPicker* paramCombo[] = { ui.trackParam_1, ui.trackParam_2, ui.trackParam_3, ui.trackParam_4 };

	for (auto combo : paramCombo) {
		for (int i = 0; i < 57; i++) {
			combo->addItem(AN1x::getFreeEGParameters(i));
		}
	}

	for (int i = 0; i < 95; i++) ui.lengthCombo->addItem(AN1x::getFreeEGLength(i));

	ui.lengthCombo->setCurrentIndex(38);

	for (int i = 0; i < ui_controls.size(); i++)
	{
		if (ui_controls[i] == nullptr) continue;

		ui_controls[i]->setCurrentValueAsDefault();
		ui_controls[i]->setParam(AN1x::ParamType::Common, (AN1x::CommonParam)i + AN1x::FreeEGTrigger);
	}

}

FreeEGWidget::~FreeEGWidget()
{}

void FreeEGWidget::setCommonParameter(AN1x::CommonParam p, int value)
{
	int idx = p - AN1x::FreeEGTrigger;

	if (idx >= 0 && idx < ui_controls.size() && ui_controls[idx] != nullptr) {
		ui_controls[idx]->setValue(value);
	}
}
