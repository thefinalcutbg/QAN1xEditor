#include "UiControlers.h"
#include <QMouseEvent>
#include <qtooltip.h>
#include <qdebug.h>
#include "Model/MidiMaster.h"
#include <QEvent>
#include <qtooltip.h>
#include <QMouseEvent>
#include "ValueDialog.h"
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
			ValueDialog d;
			d.setValue(minimum(), maximum(), defaultValue, value());
			d.exec();
			auto result = d.getValue();

			setValue(result);

			emit valueChanged(result);
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
			result += QString::number(value());
			break;

		case Frequency:
			result += AN1x::getFrequencyByValue(value());
			break;

	
		case Resonance:
		{
			QString number = QString::number(value());
			number.insert(value() < 100 ? 1:2, '.');
			result += number;
		}
		break;

		case Gain:
		{
			int number = value() - 64;
			if (number > 0) {
				result += " +";
			}
			result += QString::number(value() - 64);
			result += " db";
		}
		break;

		case LFOFrquency:
			result += AN1x::getLFOFreqByValue(value());
			break;

		case ChorusType:
			result += AN1x::getChorusTypeByValue(value());
			break;

		case Percent:
			result += QString::number(value());
			result += "%";
			break;

		case Milliseconds:
		{
			if (value() == 0) {
				result += "0.0";
			}
			else{
				QString number = QString::number(value());
				number.insert(value() < 10 ? 1 : 2, '.');
				result += number;
			}

			result += " ms";
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
			ValueDialog d;
			d.setValue(minimum(), maximum(), defaultValue, value());
			d.exec();
			auto result = d.getValue();

			setValue(result);

			emit valueChanged(result);
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
