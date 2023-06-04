#include "ValueDialog.h"

ValueDialog::ValueDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.resetButton, &QPushButton::clicked, [this] { ui.spinBox->setValue(m_default); accept(); });
	connect(ui.okButton, &QPushButton::clicked, [this] { accept(); });

}

void ValueDialog::setValue(int min, int max, int dafaultValue, int current)
{
	ui.spinBox->setMaximum(max);
	ui.spinBox->setMinimum(min);
	m_default = dafaultValue;
	m_current = current;
	ui.spinBox->setValue(current);
}

int ValueDialog::getValue()
{
	return result() == Accepted ? ui.spinBox->value() : m_current;
}

ValueDialog::~ValueDialog()
{}
