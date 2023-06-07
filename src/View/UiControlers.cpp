#include "UiControlers.h"
#include <QMouseEvent>
#include <qtooltip.h>
#include <qdebug.h>
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
		}
		return true;
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




	}

	return result;
}

DialKnob::DialKnob(QWidget *parent)
	: QDial(parent)
{
	setMouseTracking(true);
	setAttribute(Qt::WA_Hover, true);
	installEventFilter(this);

}

DialKnob::~DialKnob()
{}

void DialKnob::setSceneParam(MidiMaster* m, AN1x::SceneParam p, bool isScene2)
{
	connect(this, &QDial::valueChanged, [=](int value) {
			GlobalWidgets::statusBar->showMessage(getValueText());
			m->setSceneParam(p, value, isScene2);
		}
	);
}

void DialKnob::setCommonParam(MidiMaster* m, AN1x::CommonParam p)
{
	connect(this, &QDial::valueChanged, [=](int value) {
		GlobalWidgets::statusBar->showMessage(getValueText());
		m->setCommonParam(p, value);
		}
	);
}

void DialKnob::setCurrentValueAsDefault()
{
	defaultValue = value();
}

void DialKnob::setValue(int value)
{
	
	blockSignals(true);

	QDial::setValue(value);

	if (underMouse() && GlobalWidgets::statusBar) {
		GlobalWidgets::statusBar->showMessage(getValueText());
	}

	blockSignals(false);
}


ComboPicker::ComboPicker(QWidget* parent) : QComboBox(parent)
{

}

void ComboPicker::setSceneParam(MidiMaster* m, AN1x::SceneParam p, bool isScene2)
{
	connect(this, &QComboBox::currentIndexChanged, [=](int value) {
			//GlobalWidgets::statusBar->showMessage("Current value: " + QString::number(value));
			m->setSceneParam(p, value, isScene2);
		}
	);
}

void ComboPicker::setCommonParam(MidiMaster* m, AN1x::CommonParam p)
{
	connect(this, &QComboBox::currentIndexChanged, [=](int value) {
			//GlobalWidgets::statusBar->showMessage("Current value: " + QString::number(value));
			m->setCommonParam(p, value);
		}
	);
}

void ComboPicker::setCurrentValueAsDefault()
{
	defaultValue = currentIndex();
}

void ComboPicker::setValue(int value)
{
	blockSignals(true);
	setCurrentIndex(value);
	blockSignals(false);
}


EGSlider::EGSlider(QWidget* parent) : QSlider(parent)
{
	setMouseTracking(true);
	setAttribute(Qt::WA_Hover, true);
	installEventFilter(this);
}

void EGSlider::setSceneParam(MidiMaster* m, AN1x::SceneParam p, bool isScene2)
{
	connect(this, &QSlider::valueChanged, [=](int value) {
			GlobalWidgets::statusBar->showMessage("Current value: " + QString::number(value));
			m->setSceneParam(p, value, isScene2);
		}
	);
}

void EGSlider::setCommonParam(MidiMaster* m, AN1x::CommonParam p)
{
	connect(this, &QSlider::valueChanged, [=](int value) {
			GlobalWidgets::statusBar->showMessage("Current value: " + QString::number(value));
			m->setCommonParam(p, value);
		}
	);
}

void EGSlider::setCurrentValueAsDefault()
{
	defaultValue = value();
}

void EGSlider::setValue(int value)
{
	blockSignals(true);

	QSlider::setValue (value);

	if (underMouse() && GlobalWidgets::statusBar) {
		GlobalWidgets::statusBar->showMessage("Current value: " + QString::number(value));
	}
	blockSignals(false);
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

}

void CheckBox::setSceneParam(MidiMaster* m, AN1x::SceneParam p, bool isScene2)
{
	connect(this, &QCheckBox::stateChanged, [=](bool checked) {
		m->setSceneParam(p, checked, isScene2);
		}
	);
}

void CheckBox::setCommonParam(MidiMaster* m, AN1x::CommonParam p)
{
	connect(this, &QCheckBox::stateChanged, [=](bool checked) {
			m->setCommonParam(p, checked);
		}
	);
}

void CheckBox::setCurrentValueAsDefault()
{
	defaultValue = isChecked();
}

void CheckBox::setValue(int value)
{
	blockSignals(true);

	QCheckBox::setChecked(value);

	blockSignals(false);
}
