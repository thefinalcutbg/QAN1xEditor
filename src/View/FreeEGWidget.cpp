#include "FreeEGWidget.h"
#include "Model/MidiMaster.h"
#include "FreeEG/FreeEGScene.h"


FreeEGWidget::FreeEGWidget(QWidget* parent)
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

    for (size_t i = 0; i < ui_controls.size(); i++)
	{
		if (ui_controls[i] == nullptr) continue;

		ui_controls[i]->setCurrentValueAsDefault();
		ui_controls[i]->setParam(ParamType::Common, (AN1x::CommonParam)i + AN1x::FreeEGTrigger);
	}

	scene = new FreeEGScene(ui.egView);
	ui.egView->setScene(scene);
	ui.egView->viewport()->setMouseTracking(true);

    connect(ui.currentTrackCombo, &QComboBox::currentIndexChanged, this, [=](int index) { scene->setCurrentIndex(index); });

	scene->setCurrentIndex(0);

	ui.groupTrack_1->setStyleSheet("QGroupBox {font-weight: bold; color: blue;}");
	ui.groupTrack_2->setStyleSheet("QGroupBox {font-weight: bold; color: lightgreen;}");
	ui.groupTrack_3->setStyleSheet("QGroupBox {font-weight: bold; color: red;}");
	ui.groupTrack_4->setStyleSheet("QGroupBox {font-weight: bold; color: goldenrod;}");

	ui.keyTrack->showPlusOnPositives(true);

	connect(scene, &FreeEGScene::editingFinished, this, [&] { MidiMaster::FreeEGChanged(getFreeEGData()); });
    connect(ui.resetMode, &QComboBox::currentIndexChanged, this, [&](int resetToZero){ scene->setResetMode(resetToZero); });

	connect(ui.fromSpin, &QSpinBox::valueChanged, this, [&](int value) {
		if (value > ui.toSpin->value()) ui.toSpin->setValue(value);
	});

	connect(ui.toSpin, &QSpinBox::valueChanged, this, [&](int value) {
		if (value < ui.fromSpin->value()) ui.fromSpin->setValue(value);
	});

	connect(ui.assignButton, &QPushButton::clicked, this, [&] {
		scene->quickAssign(ui.fromSpin->value(), ui.toSpin->value(), ui.valueSpin->value());
	});
}

FreeEGWidget::~FreeEGWidget()
{}

void FreeEGWidget::setCommonParameter(AN1x::CommonParam p, int value)
{
    size_t idx = p - AN1x::FreeEGTrigger;

	if (
		p == AN1x::FreeEgTrackSceneSw1 ||
		p == AN1x::FreeEgTrackSceneSw2 ||
		p == AN1x::FreeEgTrackSceneSw3 ||
		p == AN1x::FreeEgTrackSceneSw4
		)
	{
		value /= 5; //AN1x bug - returns 0, 5, 10, 15 instead of 0,1,2,3
	}

	if (idx >= 0 && idx < ui_controls.size() && ui_controls[idx] != nullptr) {
		ui_controls[idx]->setValue(value);
	}
}

std::vector<int> FreeEGWidget::getFreeEGData()
{
	return scene->getFreeEGData();
}

void FreeEGWidget::setTrackData(const std::vector<int>& data)
{
	scene->setTrackData(data);
}

