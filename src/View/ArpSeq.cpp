#include "ArpSeq.h"

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

	setArpLayout(true);

	connect(ui.radioArp, &QRadioButton::clicked, [this] { setArpLayout(true); });
	connect(ui.radioSeq, &QRadioButton::clicked, [this] { setArpLayout(false); });
}

void ArpSeq::setMidiMaster(MidiMaster* m)
{
}

void ArpSeq::setArpLayout(bool arp)
{
	ui.arpType->setDisabled(!arp);
	ui.arpSubdivide->setDisabled(!arp);
	ui.arpLabel_1->setDisabled(!arp);
	ui.arpLabel_2->setDisabled(!arp);
	
	
	ui.seqPatternNo->setDisabled(arp);
	ui.swing->setDisabled(arp);
	ui.gate->setDisabled(arp);
	ui.seqLabel_1->setDisabled(arp);
	ui.seqLabel_2->setDisabled(arp);
	ui.seqLabel_3->setDisabled(arp);

}

ArpSeq::~ArpSeq()
{}
