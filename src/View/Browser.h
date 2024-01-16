#pragma once

#include <QWidget>
#include "ui_Browser.h"
#include "BrowserTableModel.h"
#include <QSortFilterProxyModel>

class Browser : public QWidget
{
	Q_OBJECT

	BrowserTableModel model;
	QSortFilterProxyModel column_sort;
	QSortFilterProxyModel search;
public:
	Browser(QWidget *parent = nullptr);

	void setPatchName(int idx, const std::string& name);

	QString generatePatchText(int index, const char* name);

	void setProgressBarCount(int count);
	void incrementProgressBar();

	void setPatchesToTableView(const std::vector<AN1xPatch> patches) {
		model.setData(patches);
		search.setFilterKeyColumn(2);
	};

	~Browser();

private:

	std::vector<int> getSelectedIndexes();
	void importAN1FileButtonClicked();

	Ui::BrowserClass ui;
};
