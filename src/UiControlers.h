#pragma once

#include <QDial>
#include "AbstractSceneController.h"

class DialKnob  : public QDial, public AbstractSceneController
{
	Q_OBJECT

	int m_offset{ 0 };
	QString m_suffix = "";
	bool m_showPositive{ false };

	bool event(QEvent* e) override;
	QString getValueText();

public:

	enum ValueTextType { 
		Normal, 
		EqFrequency, 
		Resonance, 
		LFOFrquency, 
		ChorusType, 
		Milliseconds, 
		Stage,
		Diffusion,
		PanDirection,
		CompressorAttack,
		CompressorRelease,
		CompressorRatio,
		WahCutoffFreq,
		AMPType,
		DelayInput,
		TempoDelay,
		ReverbTime,
		ReverbDamp
	};

private: ValueTextType m_textType{ Normal };

public:
	DialKnob(QWidget *parent);
	
	~DialKnob();

	// Inherited via AbstractSceneController

	void setSceneParam(MidiMaster* m, AN1x::SceneParam p, bool isScene2) final;
	void setCommonParam(MidiMaster* m, AN1x::CommonParam p) final;
	void setCurrentValueAsDefault() final;

	void setValue(int value) final;
	void resetValueText() {
		m_textType = ValueTextType::Normal;
		m_suffix.clear();
		m_offset = 0;
		m_showPositive = false;
	}

	void setValueTextType(ValueTextType t) { m_textType = t;  };
	void setOffset(int offset) { m_offset = offset; };
	void setSuffix(const char* suffix) { m_suffix = suffix; }
	void setShowPositive(bool show) { m_showPositive = show; }
};

#include <QComboBox>

class ComboPicker : public QComboBox, public AbstractSceneController
{
	Q_OBJECT

public:
	ComboPicker(QWidget* parent);

	void setSceneParam(MidiMaster* m, AN1x::SceneParam p, bool isScene2) final;
	void setCommonParam(MidiMaster* m, AN1x::CommonParam p) final;
	void setCurrentValueAsDefault() final;
	void setValue(int value) final;

	~ComboPicker() {};
};

#include <QSlider>

class EGSlider : public QSlider, public AbstractSceneController
{
	Q_OBJECT

	bool event(QEvent* e) override;

public:
	EGSlider(QWidget* parent);

	void setSceneParam(MidiMaster* m, AN1x::SceneParam p, bool isScene2) final;
	void setCommonParam(MidiMaster* m, AN1x::CommonParam p) final;
	void setCurrentValueAsDefault() final;
	void setValue(int value) final;

	~EGSlider() {};
};

#include <QCheckBox>

class CheckBox : public QCheckBox, public AbstractSceneController
{
	Q_OBJECT

		//bool event(QEvent* e) override;

public:
	CheckBox(QWidget* parent);

	void setSceneParam(MidiMaster* m, AN1x::SceneParam p, bool isScene2) final;
	void setCommonParam(MidiMaster* m, AN1x::CommonParam p) final;
	void setCurrentValueAsDefault() final;
	void setValue(int value) final;

	~CheckBox() {};
};