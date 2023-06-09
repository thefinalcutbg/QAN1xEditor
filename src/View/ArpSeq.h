#pragma once

#include <QWidget>
#include "ui_ArpSeq.h"

class ArpSeq : public QWidget
{
	Q_OBJECT

		
	static constexpr int ui_size = AN1x::NullCommon23 - AN1x::ArpSeqOnOff;

	std::array<AbstractController*, ui_size> ui_controls{ nullptr };
	std::array<AbstractController*, AN1x::StepSequencerMaxSize> seq_controls{ nullptr };
	std::array<QGroupBox*, 16> p_steps;

	void setArpLayout(bool arp);

	
public:
	ArpSeq(QWidget *parent = nullptr);
	void setCommonParameter(AN1x::CommonParam p, int value);
	void setSequenceParameter(AN1x::SeqParam p, int value);
	~ArpSeq();

private:
	Ui::ArpSeqClass ui;
};
