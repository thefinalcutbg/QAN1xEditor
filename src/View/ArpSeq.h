#pragma once

#include <QWidget>
#include "ui_ArpSeq.h"
#include "AbstractSceneController.h"

class ArpSeq : public QWidget
{
	Q_OBJECT

		
	static constexpr int ui_size = AN1x::NullCommon23 - AN1x::ArpSeqOnOff;

	std::array<AbstractSceneController*, ui_size> ui_controls{ nullptr };
	std::array<AbstractSceneController*, AN1x::SeqNote1> seq_controls{ nullptr };

	void setArpLayout(bool arp);

	
public:
	ArpSeq(QWidget *parent = nullptr);
	void setMidiMaster(MidiMaster* m);
	void setCommonParameter(AN1x::CommonParam p, int value);
	void setSequenceParameter(AN1x::StepSequencer p, int value);
	~ArpSeq();

private:
	Ui::ArpSeqClass ui;
};
