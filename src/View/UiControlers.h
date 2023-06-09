#pragma once

#include <QDial>
#include "AbstractController.h"

class DialKnob  : public QDial, public AbstractController
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

	// Inherited via AbstractController

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
	void showPlusOnPositives(bool show) { m_showPositive = show; }
};

#include <QComboBox>

class ComboPicker : public QComboBox, public AbstractController
{
	Q_OBJECT

	bool m_isNoteCombo{ false };

public:
	ComboPicker(QWidget* parent);

	void setCurrentValueAsDefault() final;
	void setValue(int value) final;
	void setAsNoteCombo() { m_isNoteCombo = true; }
	~ComboPicker() {};
};

#include <QSlider>

class EGSlider : public QSlider, public AbstractController
{
	Q_OBJECT

	bool event(QEvent* e) override;

public:
	EGSlider(QWidget* parent);

	void setCurrentValueAsDefault() final;
	void setValue(int value) final;

	~EGSlider() {};
};

#include <QCheckBox>

class CheckBox : public QCheckBox, public AbstractController
{
	Q_OBJECT

		//bool event(QEvent* e) override;

public:
	CheckBox(QWidget* parent);

	void setCurrentValueAsDefault() final;
	void setValue(int value) final;

	~CheckBox() {};
};

#include <QSpinBox>

class SpinBox : public QSpinBox, public AbstractController
{
	Q_OBJECT

public:
	SpinBox(QWidget* parent);
	void setCurrentValueAsDefault() final;
	void setValue(int value) final;
};