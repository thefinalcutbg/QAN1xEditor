#pragma once

#include <QWidget>
#include "ui_Browser.h"

class Browser : public QWidget
{
	Q_OBJECT

public:
	Browser(QWidget *parent = nullptr);

	void setPatchName(int idx, const std::string& name);

	QString generatePatchText(int index, const char* name);

	void setProgressBarCount(int count);
	void incrementProgressBar();

	~Browser();

private:

	std::vector<int> getSelectedIndexes();

	Ui::BrowserClass ui;
};
