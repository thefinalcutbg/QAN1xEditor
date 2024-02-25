#pragma once

#include <QWidget>
#include "ui_Browser.h"
#include "BrowserTableModel.h"
#include <QSortFilterProxyModel>
#include <set>
class Browser : public QWidget
{
	Q_OBJECT

	BrowserTableModel model;
	QSortFilterProxyModel column_sort;
	QSortFilterProxyModel search;
public:
	Browser(QWidget *parent = nullptr);

	void setPatchToListView(int idx, const std::string& name, int type);

	QString generatePatchText(int index, const char* name);

	void setProgressBarCount(int count);
	void incrementProgressBar();

	void setPatchesToTableView(const std::vector<PatchRow>& patches);

	void scrollToBottom();

	~Browser();

private:

	void recalculateListNames();
	std::vector<int> getSelectedListIndexes();
	std::set<long long> getSelectedTableRowids();
	void importAN1FileButtonClicked();
	void importAN2FileButtonClicked();
	void loadAN1FileToList();
	void exportAN2File();
	void editComment();
	void disableWidgets(bool disabled);
	void saveAN1File();

	Ui::BrowserClass ui;
};
