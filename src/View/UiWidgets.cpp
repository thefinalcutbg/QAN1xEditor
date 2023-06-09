#include "UiWidgets.h"
#include <QMouseEvent>
#include <qtooltip.h>

#include "Model/MidiMaster.h"
#include <QEvent>
#include <qtooltip.h>
#include <QMouseEvent>

#include "GlobalWidgets.h"
#include <QStatusBar>

bool DialKnob::event(QEvent* e)
{

	switch (e->type())
	{
	case QEvent::HoverEnter:
		GlobalWidgets::statusBar->showMessage(getValueText());
		return true;
		break;
	case QEvent::HoverLeave:
		GlobalWidgets::statusBar->clearMessage();
		return true;
		break;
	case QEvent::MouseButtonPress:
		if (static_cast<QMouseEvent*>(e)->button() == Qt::RightButton)
		{
			setValue(defaultValue);

			emit valueChanged(value());
			return true;
		}
		break;
	default:
		break;
	}
	return QDial::event(e);
}

QString DialKnob::getValueText()
{
	QString result = "Current value: ";

	switch(m_textType)
	{ 
		case Normal:
		{
			auto val = value() + m_offset;
			if (m_showPositive && val > 0) result += "+";
			result += QString::number(val);

			result += m_suffix;
		}	
		break;

		case EqFrequency:
			result += AN1x::getFrequencyByValue(value());
			break;

		case Resonance:
		{
			auto val = value() + m_offset;
			QString number = QString::number(val);
			number.insert(value() < 100 ? 1:2, '.');
			result += number;
		}
		break;

		case Milliseconds:
		{
			auto val = value() + m_offset;
			if (val == 0) {
				result += "0.0";
			}
			else{

				QString number = QString::number(val);
				if (val < 10) number.insert(0, "0.");
				else number.insert(number.size() - 1, '.');
				
				result += number;
			}

			result += " ms";
		}
			break;

		case ReverbDamp:
		{
			//same as Milliseconds, but no suffix
			auto val = value() + m_offset;
			if (val == 0) {
				result += "0.0";
			}
			else {

				QString number = QString::number(val);
				if (val < 10) number.insert(0, "0.");
				else number.insert(number.size() - 1, '.');

				result += number;
			}
		}
		break;


		case LFOFrquency:
			result += AN1x::getLFOFreqByValue(value());
			break;
		case ChorusType:
			result += AN1x::getChorusTypeByValue(value());
			break;
		case CompressorAttack:
			result += QString::number(AN1x::compressorAttack(value()));
			break;
		case CompressorRelease:
			result += QString::number(AN1x::compressorRelease(value()));
			break;
		case CompressorRatio:
			result += AN1x::compressorRatio(value());
			break;
		case WahCutoffFreq:
			result += AN1x::wahCutoffFreq(value());
			break;
		case ReverbTime:
			result += AN1x::reverbTime(value());
			result += "s";
			break;


		case Stage:
		{
			auto val = value();
			int values[] = { 4,6,8 };
			result += QString::number(values[val]);
		}
		break;

		case Diffusion:
			result += value() ? "Stereo" : "Mono";
			break;

		case PanDirection:
		{
			const char* values[] =
			{
				"Left->Right", "Left<-Right","Left<->Right","Left turn", "Right turn"
			};
			result += values[value()];
		}
		break;

		case AMPType:
		{
			const char* values[] =
			{
				"Off", "Stack", "Combo", "Tube"
			};
			result += values[value()];
		}	
		break;

		case DelayInput:
		{
			const char* values[] =
			{
				"Left", "Right", "Left & Right"
			};
			result += values[value()];
		}
		break;

		case TempoDelay:
		{
			const char* values[] =
			{
				"1/2","3/8","1/4","3/16","1/6","1/8","3/32","1/12","1/16","1/24","1/32"
			};
			result += values[value()];
		}
		break;

		case MasterTune:
		{
			return ""; //implement this
		}
		break;


	}

	return result;
}

DialKnob::DialKnob(QWidget *parent)
	: QDial(parent)
{
	setMouseTracking(true);
	setAttribute(Qt::WA_Hover, true);
	installEventFilter(this);

	connect(this, &QDial::valueChanged, 
		[&](int value) {
			if (type == AN1x::ParamType::Unknown) return;
			GlobalWidgets::statusBar->showMessage(getValueText());
			MidiMaster::setParam(type, parameter, value);
		}
	);

}

DialKnob::~DialKnob()
{}

void DialKnob::setCurrentValueAsDefault()
{
	defaultValue = value();
}

void DialKnob::setValue(int value)
{
	
	//blockSignals(true);

	QDial::setValue(value);

	if (underMouse() && GlobalWidgets::statusBar) {
		GlobalWidgets::statusBar->showMessage(getValueText());
	}

	//blockSignals(false);
}


bool ComboPicker::event(QEvent* e)
{
	switch (e->type())
	{
	case QEvent::HoverEnter:
		GlobalWidgets::statusBar->showMessage("Current Value: " + itemText(currentIndex()));
		return true;
		break;
	case QEvent::HoverLeave:
		GlobalWidgets::statusBar->clearMessage();
		return true;
		break;
	case QEvent::MouseButtonPress:
		if (static_cast<QMouseEvent*>(e)->button() == Qt::RightButton)
		{
			int val = m_isNoteCombo ? 60 : defaultValue;
			setValue(val);

			emit currentIndexChanged(val);
			return true;
		}
		
		break;
	default:
		break;
	}

	return QComboBox::event(e);
}

ComboPicker::ComboPicker(QWidget* parent) : QComboBox(parent)
{
	connect(this, &QComboBox::currentIndexChanged, 
		[&](int value) { 
			if (type == AN1x::ParamType::Unknown) return;

			GlobalWidgets::statusBar->showMessage("Current Value: " + itemText(value));
			if (m_isNoteCombo) value = 127 - value;
			MidiMaster::setParam(type, parameter, value); }
	);

	installEventFilter(this);
}



void ComboPicker::setCurrentValueAsDefault()
{
	defaultValue = currentIndex();
}

void ComboPicker::setValue(int value)
{
	blockSignals(true);
	if (m_isNoteCombo) value = 127-value;
	setCurrentIndex(value);
	blockSignals(false);
}

void ComboPicker::setAsNoteCombo()
{
	blockSignals(true);
	m_isNoteCombo = true;

	clear();

	static const char* notelist[12] = { "B","A#","A","G#","G","F#","F","E","D#","D","C#","C" };

	for (int y = 8; y >= -2; y--) {
		for (int j = 0; j < 12; j++)
		{
			if (y == 8 && j < 4) continue;
			addItem(QString::number(y) + notelist[j]);
		}
	}

	setCurrentIndex(127 - 60);

	blockSignals(false);
}


EGSlider::EGSlider(QWidget* parent) : QSlider(parent)
{
	setMouseTracking(true);
	setAttribute(Qt::WA_Hover, true);
	installEventFilter(this);

	connect(this, &QSlider::valueChanged, [&](int value) {
		if (type == AN1x::ParamType::Unknown) return;
		GlobalWidgets::statusBar->showMessage("Current value: " + QString::number(value));
			MidiMaster::setParam(type, parameter, value);
		}
	);
}

void EGSlider::setCurrentValueAsDefault()
{
	defaultValue = value();
}

void EGSlider::setValue(int value)
{
	//blockSignals(true);

	QSlider::setValue (value);

	if (underMouse() && GlobalWidgets::statusBar) {
		GlobalWidgets::statusBar->showMessage("Current value: " + QString::number(value));
	}
	//blockSignals(false);
}

bool EGSlider::event(QEvent* e)
{
	switch (e->type())
	{
	case QEvent::HoverEnter:
		GlobalWidgets::statusBar->showMessage("Current value: " + QString::number(value()));
		return true;
		break;
	case QEvent::HoverLeave:
		GlobalWidgets::statusBar->clearMessage();
		return true;
		break;
	case QEvent::MouseButtonPress:
		if (static_cast<QMouseEvent*>(e)->button() == Qt::RightButton)
		{
			setValue(defaultValue);

			emit valueChanged(value());
			return true;
		}

		break;
	default:
		break;
	}

	return QSlider::event(e);
}

CheckBox::CheckBox(QWidget* parent) : QCheckBox(parent)
{
	connect(this, &QCheckBox::stateChanged, [&](bool checked) {
			if (type == AN1x::ParamType::Unknown) return;
			MidiMaster::setParam(type, parameter, checked);
		}
	);
}

void CheckBox::setCurrentValueAsDefault()
{
	defaultValue = isChecked();
}

void CheckBox::setValue(int value)
{
	//blockSignals(true);

	QCheckBox::setChecked(value);

	//blockSignals(false);
}

SpinBox::SpinBox(QWidget* parent) : QSpinBox(parent)
{
	connect(this, &QSpinBox::valueChanged, [&](int value) {
			if (type == AN1x::ParamType::Unknown) return;
			MidiMaster::setParam(type, parameter, value);
		}
	);
}

void SpinBox::setCurrentValueAsDefault()
{
	defaultValue = value();
}

void SpinBox::setValue(int value)
{
	//blockSignals(true);
	QSpinBox::setValue(value);
	//blockSignals(false);
}
