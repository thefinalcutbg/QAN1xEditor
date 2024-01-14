#pragma once

#include <QWidget>
#include "ui_Browser.h"

class Browser : public QWidget
{
	Q_OBJECT

public:
	Browser(QWidget *parent = nullptr);

	void setPatchName(int idx, const std::string& name);

	~Browser();

private:

	std::vector<int> getSelectedIndexes();

	QString generatePatchText(int index, const char* name);

	Ui::BrowserClass ui;
};
