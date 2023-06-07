#include "ArpSeq.h"
#include "Model/MidiMaster.h";

ArpSeq::ArpSeq(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui_controls = {
		nullptr, //arp/seq select
		nullptr, //arptype - seq number
		ui.kbdMode,
		ui.stepHoldCombo,
		nullptr, //select scene
		ui.arpSubdivide,
		ui.swing,
		ui.velocity,
		ui.gate
	};
	
	ui.swing->setSuffix("%");
	ui.swing->setOffset(18);
	
	ui.velocity->setSuffix("%");
	ui.gate->setSuffix("%");
	

	setArpLayout(true);

	connect(ui.radioArp, &QRadioButton::clicked, [this] { setArpLayout(true); });
	connect(ui.radioSeq, &QRadioButton::clicked, [this] { setArpLayout(false); });
	connect(ui.kbdMode, &QComboBox::currentIndexChanged, [this](int index) { 
		ui.seqPatternNo->setDisabled(index < 2); 
		ui.seqLabel_1->setDisabled(index < 2);
	});

	
}

void ArpSeq::setMidiMaster(MidiMaster* m)
{

	connect(ui.radioArp, &QRadioButton::clicked, [=] { m->setCommonParam(AN1x::ArpSeqSelect, 0); });
	connect(ui.radioSeq, &QRadioButton::clicked, [=] { m->setCommonParam(AN1x::ArpSeqSelect, 1); });
	connect(ui.radioScene1, &QRadioButton::clicked, [=] { m->setCommonParam(AN1x::SceneSelect, 0); });
	connect(ui.radioScene2, &QRadioButton::clicked, [=] { m->setCommonParam(AN1x::SceneSelect, 1); });
	connect(ui.radioBothScenes, &QRadioButton::clicked, [=] { m->setCommonParam(AN1x::SceneSelect, 0); });
	connect(ui.arpType, &QComboBox::currentIndexChanged, [=](int index) { m->setCommonParam(AN1x::ArpTypeSeqNo, index); });
	connect(ui.seqPatternNo, &QSpinBox::valueChanged, [=](int value) { m->setCommonParam(AN1x::ArpTypeSeqNo, value-1); });

	for (int i = 0; i < ui_controls.size(); i++)
	{
		if (ui_controls[i] == nullptr) continue;

		auto param = AN1x::ArpSeqSelect + i;

		ui_controls[i]->setCurrentValueAsDefault();
		ui_controls[i]->setCommonParam(m, (AN1x::CommonParam)param);
	}
}

void ArpSeq::setCommonParameter(AN1x::CommonParam p, int value)
{
	if (p < AN1x::CommonParam::ArpSeqSelect || p >= AN1x::CommonParam::NullCommon23) return;

	switch (p)
	{
	case AN1x::ArpSeqSelect:
		value ? ui.radioSeq->click() : ui.radioArp->click(); 
		break;

	case AN1x::ArpSeqScene:

		switch (value) {
			case 0: ui.radioScene1->click(); break;
			case 2: ui.radioScene1->click(); break;
			case 3: ui.radioScene1->click(); break;
		}
		break;
	
	case AN1x::ArpTypeSeqNo:
		ui.radioArp->isChecked() ?
			ui.arpType->setCurrentIndex(value)
			:
			ui.seqPatternNo->setValue(value + 1);
		break;

	default:
		ui_controls[static_cast<int>(p - AN1x::CommonParam::ArpSeqSelect)]->setValue(value);
	
	}

}

void ArpSeq::setArpLayout(bool arp)
{
	ui.arpType->setDisabled(!arp);
	ui.arpSubdivide->setDisabled(!arp);
	ui.arpLabel_1->setDisabled(!arp);
	ui.arpLabel_2->setDisabled(!arp);
	
	ui.swing->setDisabled(arp);
	ui.gate->setDisabled(arp);
	
	ui.seqLabel_2->setDisabled(arp);
	ui.seqLabel_3->setDisabled(arp);

	if (arp) {

		ui.seqPatternNo->setDisabled(true);
		ui.seqLabel_1->setDisabled(true);

		ui.kbdMode->setItemText(0, "Chord");
		ui.kbdMode->setItemText(1, "Chord & Normal");

		if (ui.kbdMode->count() > 2) {
			ui.kbdMode->removeItem(2);
			ui.kbdMode->removeItem(2);
		}
	}
	else {

		ui.kbdMode->setItemText(0, "Normal");
		ui.kbdMode->setItemText(1, "Note Shift & Normal");

		if (ui.kbdMode->count() < 3)
		{
			ui.kbdMode->addItem("Pt Select & Normal");
			ui.kbdMode->addItem("Pt Select & Note Shift");
		}

		bool isPattern = ui.kbdMode->currentIndex() > 1;

		ui.seqPatternNo->setDisabled(!isPattern);
		ui.seqLabel_1->setDisabled(!isPattern);

	}
	
}

ArpSeq::~ArpSeq()
{}
