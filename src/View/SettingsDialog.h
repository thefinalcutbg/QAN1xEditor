#pragma once

#include <QDialog>
#include <optional>
#include "ui_SettingsDialog.h"

#include "Model/Settings.h"

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	SettingsDialog(const AdvancedMidiSettings& s);
	~SettingsDialog();

	std::optional<AdvancedMidiSettings> result;

private:
	Ui::SettingsDialogClass ui;
};
