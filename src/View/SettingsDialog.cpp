#include "SettingsDialog.h"

SettingsDialog::SettingsDialog(const AdvancedMidiSettings& s) : QDialog(nullptr)
{
    ui.setupUi(this);

	setWindowTitle("MIDI Settings");

    ui.deviceSpin->setMinimumWidth(40);

	ui.buffer->setValue(s.buffer_size);
	ui.thruCheck->setChecked(s.midi_thru);
	ui.delay->setValue(s.msDelay);
	ui.deviceSpin->setValue(s.device_no);
	ui.sendSpin->setValue(s.midi_send_channel);
	ui.deviceSpin->setSpecialValueText("All");

	connect(ui.okButton, &QPushButton::clicked, this, [&] {

		result = AdvancedMidiSettings{
		.midi_send_channel = ui.sendSpin->value(),
		.midi_thru = ui.thruCheck->isChecked(),
		.device_no = ui.deviceSpin->value(),
		.buffer_size = ui.buffer->value(),
		.msDelay = ui.delay->value()
		};

		close();
	});

}

SettingsDialog::~SettingsDialog()
{}
