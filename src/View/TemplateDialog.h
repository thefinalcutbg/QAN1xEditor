#pragma once

#include <QDialog>
#include <vector>

#include "ui_TemplateDialog.h"

class TemplateDialog : public QDialog
{
	Q_OBJECT

public:
	explicit TemplateDialog(int type, QWidget* parent = nullptr);
	~TemplateDialog();

	std::vector<unsigned char> getResult() const;

private:
	void refreshList();
	void loadCurrent();
	void deleteCurrent();

private:
	Ui::TemplateDialogClass ui;
	int m_type;
	std::vector<unsigned char> m_result;
	bool m_loadPressed = false;
};