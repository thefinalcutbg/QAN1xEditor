#include "ArpSeq.h"
#include "Model/MidiMaster.h"

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

	seq_controls = {
		ui.baseUnit,
		ui.seqLength,
		ui.loopType,
		ui.controlChange,
		nullptr,
		nullptr,
		ui.note_1,
		ui.note_2,
		ui.note_3,
		ui.note_4,
		ui.note_5,
		ui.note_6,
		ui.note_7,
		ui.note_8,
		ui.note_9,
		ui.note_10,
		ui.note_11,
		ui.note_12,
		ui.note_13,
		ui.note_14,
		ui.note_15,
		ui.note_16,
		ui.velocity_1,
		ui.velocity_2,
		ui.velocity_3,
		ui.velocity_4,
		ui.velocity_5,
		ui.velocity_6,
		ui.velocity_7,
		ui.velocity_8,
		ui.velocity_9,
		ui.velocity_10,
		ui.velocity_11,
		ui.velocity_12,
		ui.velocity_13,
		ui.velocity_14,
		ui.velocity_15,
		ui.velocity_16,
		ui.gate_1,
		ui.gate_2,
		ui.gate_3,
		ui.gate_4,
		ui.gate_5,
		ui.gate_6,
		ui.gate_7,
		ui.gate_8,
		ui.gate_9,
		ui.gate_10,
		ui.gate_11,
		ui.gate_12,
		ui.gate_13,
		ui.gate_14,
		ui.gate_15,
		ui.gate_16,
		ui.ctrl_1,
		ui.ctrl_2,
		ui.ctrl_3,
		ui.ctrl_4,
		ui.ctrl_5,
		ui.ctrl_6,
		ui.ctrl_7,
		ui.ctrl_8,
		ui.ctrl_9,
		ui.ctrl_10,
		ui.ctrl_11,
		ui.ctrl_12,
		ui.ctrl_13,
		ui.ctrl_14,
		ui.ctrl_15,
		ui.ctrl_16
	};

	for (int i = AN1x::SeqNote1; i < AN1x::SeqVelocity1; i++) {

		auto note = static_cast<ComboPicker*>(seq_controls[i]);

		note->setAsNoteCombo();
	}

	for (int i = AN1x::SeqGateTime1; i < AN1x::SeqCtrlChange1; i++)
	{
		static_cast<DialKnob*>(seq_controls[i])->setSuffix("%");
	}

	for (auto ui : ui_controls) if(ui)ui->setCurrentValueAsDefault();

	p_steps = {
		ui.step_1,
		ui.step_2,
		ui.step_3,
		ui.step_4,
		ui.step_5,
		ui.step_6,
		ui.step_7,
		ui.step_8,
		ui.step_9,
		ui.step_10,
		ui.step_11,
		ui.step_12,
		ui.step_13,
		ui.step_14,
		ui.step_15,
		ui.step_16
	};

	connect(ui.seqLength, &QSpinBox::valueChanged, [this](int value) {
			for (int i = 0; i < 16; i++) p_steps[i]->setEnabled(i < value);
		}
	);
	
	ui.seqLength->setValue(8);

	QString SeqCtrlEntities[97]{
		"Off","Mod Depth", "Breath", "Scene select", "Foot", "Portamento Time", "Data MSB", "Volume", "Layer", "Poly/Mono",
		"PanPot", "Expression", "Ribbon Z", "Ribbon X", "LFO Reset", "LFO1 Wave", "LFO1 Speed", "LFO2 Speed", "VCO1 LFO Mod Depth", "VCF LFO Mod Depth",
		"LFO1 Delay", "VCO1 Tune", "Sync Pitch", "Sync Depth", "Sync Source", "PEG Depth", "PEG Scene", "PEG Decay", "PEG Sustain", "PEG Release", "VCF Key Track",
		"AEG Sustain", "--", "VCO Algorithm", "Sync Pitch Mod", "FM Depth", "FM Source 1", "FM Source 2", "Data LSB", "Mix Noise",
		"--","--","--","--","--","--","--","--","--","--","VCO1 Wave",
		"VCO2 Wave","VCO2 Tune","VCO2 Fine","VCO2 Edge","VCO2 Pulse Width","VCO2 PW Depth","VCO2 LFO Mod Depth","VCF HPF","VCF Type",
		"FEG Vel.Sens.","AEG Vel.Sens.","VCA Volume","VCA Feedback","Sustain Switch", "Portamento Switch", "--","--","VCO1 Level","VCO2 Level",
		"Ring Mod. Level", "VCF Resonance", "AEG Release", "AEG Attack", "VCF Cutoff","AEG Decay","VCO1 Edge","VCO1 Fine","VCO1 Pusle Width","VCO1 PW Depth",
		"VCA LFO Mod Depth","FEG Depth","FEG Attack","FEG Decay","--","Portamento Mode","VCO1PSRC","VCO2PSRC","--","--","Arp/Seq","Reverb Send","--","VAR D:W","Delay Send","--","AT"
	};

	for (auto& s : SeqCtrlEntities) ui.controlChange->addItem(s);
	
	ui.controlChange->setCurrentIndex(0);


	connect(ui.radioArp, &QRadioButton::clicked, [=] { MidiMaster::setParam(AN1x::ParamType::Common, AN1x::ArpSeqSelect, 0); });
	connect(ui.radioSeq, &QRadioButton::clicked, [=] { MidiMaster::setParam(AN1x::ParamType::Common, AN1x::ArpSeqSelect, 1); });
	connect(ui.radioScene1, &QRadioButton::clicked, [=] { MidiMaster::setParam(AN1x::ParamType::Common, AN1x::ArpSeqScene, 0); });
	connect(ui.radioScene2, &QRadioButton::clicked, [=] { MidiMaster::setParam(AN1x::ParamType::Common, AN1x::ArpSeqScene, 1); });
	connect(ui.radioBothScenes, &QRadioButton::clicked, [=] { MidiMaster::setParam(AN1x::ParamType::Common, AN1x::ArpSeqScene, 0); });
	connect(ui.arpType, &QComboBox::currentIndexChanged, [=](int index) { MidiMaster::setParam(AN1x::ParamType::Common, AN1x::ArpTypeSeqNo, index); });
	connect(ui.seqPatternNo, &QSpinBox::valueChanged, [=](int value) { MidiMaster::setParam(AN1x::ParamType::Common, AN1x::ArpTypeSeqNo, value - 1); });

	for (int i = 0; i < ui_controls.size(); i++)
	{
		if (ui_controls[i] == nullptr) continue;

		auto param = AN1x::ArpSeqSelect + i;

		ui_controls[i]->setCurrentValueAsDefault();
		ui_controls[i]->setParam(AN1x::ParamType::Common, (AN1x::CommonParam)param);
	}

	connect(ui.seqLength, &QSpinBox::valueChanged, [=](int value) { MidiMaster::setParam(AN1x::ParamType::StepSq, AN1x::SeqLength, value); });

	for (int i = 0; i < seq_controls.size(); i++) {

		if (seq_controls[i] == nullptr) continue;

		seq_controls[i]->setCurrentValueAsDefault();
		seq_controls[i]->setParam(AN1x::ParamType::StepSq, (AN1x::SeqParam)i);
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

void ArpSeq::setSequenceParameter(AN1x::SeqParam p, int value)
{
	if (seq_controls[p] == nullptr) return;

	if (p == AN1x::SeqLength) {
		ui.seqLength->setValue(value);
		return;
	}

	seq_controls[p]->setValue(value);
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

		if (ui.stepHoldCombo->count() > 2)
		{
			ui.stepHoldCombo->removeItem(2);
			ui.stepHoldCombo->setItemText(1, "On");
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


		if (ui.stepHoldCombo->count() < 3)
		{
			ui.stepHoldCombo->setItemText(1, "Mode 1");
			ui.stepHoldCombo->addItem("Mode 2");
		}

		bool isPattern = ui.kbdMode->currentIndex() > 1;

		ui.seqPatternNo->setDisabled(!isPattern);
		ui.seqLabel_1->setDisabled(!isPattern);

	}
	
}

ArpSeq::~ArpSeq()
{}
