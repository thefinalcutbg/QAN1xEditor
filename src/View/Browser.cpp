#include "Browser.h"

#include <algorithm>

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QScrollBar>

#include "FreeFunctions.h"
#include "GlobalWidgets.h"

#include "Model/DragDropManager.h"
#include "Model/PatchMemory.h"
#include "Model/PatchDatabase.h"
#include "Model/ClipoboardManager.h"

Browser::Browser(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	GlobalWidgets::browser = this;

	column_sort.setDynamicSortFilter(true);

	column_sort.setSourceModel(&model);
	search.setSourceModel(&column_sort);
	search.setFilterKeyColumn(5);
	ui.databaseView->setModel(&search);

	for (int i = 0; i < 5; i++) {
		ui.databaseView->hideColumn(i);
	}


	connect(ui.searchTypeCombo, &QComboBox::currentIndexChanged, [&](int index) {

		search.setFilterKeyColumn(index + 5);

		});

	connect(ui.An1xList->model(), &QAbstractListModel::rowsMoved, [&](const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row)
		{
			PatchMemory::rowMoved(start, row);

			recalculateListNames();
		});

	connect(ui.lineEdit, &QLineEdit::textChanged, [=]
		{
			QString text = ui.lineEdit->text();
			search.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::PatternOption::CaseInsensitiveOption));

		});

	connect(ui.databaseView->horizontalHeader(), &QHeaderView::sectionClicked, [&](int column) {

		const int hiddenColumnMap[] = { 0,1,2,3,4,5,1,2,3,4,10,11 };

		column_sort.sort(hiddenColumnMap[column]);

		});

	connect(ui.deleteButton, &QPushButton::clicked, [&] {

		auto selectedRowids = getSelectedTableRowids();

		if (selectedRowids.empty()) return;

		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "Warning", "Delete is permanent. Are you sure?",
			QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::No) {
			return;
		}

		PatchDatabase::deleteSelectedPatches(selectedRowids);

	});

	connect(ui.databaseView, &DbTableView::deletePressed, [&] { ui.deleteButton->click(); });

	connect(ui.databaseView, &DbTableView::copyRequested, [&] { 
			ClipboardManager::copyRequestFromDatabase(getSelectedTableRowids());
	});

	connect(ui.An1xList, &MemoryList::copyRequested, [&] {
			ClipboardManager::copyRequestFromMemoryList(getSelectedListIndexes());
		});

	connect(ui.An1xList, &MemoryList::pasteRequested, [&]{
					
				auto indexes = getSelectedListIndexes();

				if (indexes.empty()) return;

				ClipboardManager::pasteToListRequested(indexes[0]);

		});

	for (int i = 0; i < 128; i++)
	{
		ui.An1xList->addItem("");
		setPatchToListView(i, "InitNormal", 0);
	}

	connect(ui.loadButton, &QPushButton::clicked, [&] {PatchMemory::loadFromAn1x(getSelectedListIndexes());});
	connect(ui.sendButton, &QPushButton::clicked, [&] {PatchMemory::sendToAn1x(getSelectedListIndexes()); });
	connect(ui.An1xList, &QListWidget::doubleClicked, [&] {

		auto indexes = getSelectedListIndexes();

		if (indexes.empty()) return;

		PatchMemory::loadAn1xMemPatch(indexes[0]);

	});

	connect(ui.importButton, &QPushButton::clicked, [&] {
		importAN1FileButtonClicked();
	});
	
	connect(ui.databaseView, &QTableView::doubleClicked, this, [&](const QModelIndex& index) {

		int idx = search.index(index.row(), 0).data().toLongLong();
		PatchDatabase::setVoiceAsCurrent(idx);

	});
	
	connect(ui.databaseView, &DbTableView::dataDroped, [&] { 
		DragDropManager::droppedToDbTable(getSelectedListIndexes());

	});

	connect(ui.An1xList, &MemoryList::dataDroped, [&](int row) { 
		DragDropManager::droppedToMemoryList(getSelectedTableRowids(), row); 
	});

	PatchDatabase::refreshTableView();

}

void Browser::setPatchToListView(int idx, const std::string& name, int type)
{
	ui.An1xList->item(idx)->setText(generatePatchText(idx, name.c_str()));
	ui.An1xList->item(idx)->setIcon(FreeFn::getTypeIcon(type));
}

Browser::~Browser()
{}

void Browser::recalculateListNames()
{
	for (int i = 0; i < ui.An1xList->count(); i++)
	{
		auto text = ui.An1xList->item(i)->text();

		QString number = QString::number(i+1);

		if (i+1 < 10) number += ".   ";
		else if (i+1 < 100) number += ".  ";
		else number += ". ";

		for (int y = 0; y < number.size(); y++)
		{
			text[y] = number[y];
		}

		ui.An1xList->item(i)->setText(text);

	}
}

std::vector<int> Browser::getSelectedListIndexes()
{

	QModelIndexList indexes = ui.An1xList->selectionModel()->selectedIndexes();

	std::vector<int> indexList;

	for (QModelIndex& index : indexes){
		indexList.push_back(index.row());
	}
	
	return indexList;
}

std::set<long long> Browser::getSelectedTableRowids()
{
	QModelIndexList indexes = ui.databaseView->selectionModel()->selectedIndexes();

	std::set<long long> rowids;

	for (QModelIndex& index : indexes) {
		
		rowids.insert(search.index(index.row(), 0).data().toLongLong());

		if (rowids.size() > 128) break;
	}

	return rowids;

}

void Browser::importAN1FileButtonClicked()
{

	QFileDialog dialog(this);
	dialog.setDirectory(QDir::homePath());
	dialog.setFileMode(QFileDialog::ExistingFiles);
	dialog.setNameFilter("An1x file (*.an1)");
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();

	if (fileNames.empty()) return;

	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(this, "Import An1 files", "Do you want to skip duplicates?",
		QMessageBox::Yes | QMessageBox::No);

	bool skipDuplicates = reply == QMessageBox::Yes;

	setProgressBarCount(fileNames.size());

	for (auto& filePath : fileNames)
	{
		incrementProgressBar();

		if (filePath.isEmpty()) continue;

		QFile file(filePath);

		if (!file.open(QIODevice::ReadOnly)) continue;

		auto bytes = file.readAll();

		QFileInfo fileInfo(file.fileName());

		try {
			PatchDatabase::loadAn1FileToBuffer(std::vector<unsigned char>{bytes.begin(), bytes.end()}, fileInfo.fileName().toStdString());;
		}
		catch (std::exception) {
			QMessageBox msgBox;
			msgBox.setText("The file is corrupted");
			msgBox.exec();
		}

		file.close();
	}

	PatchDatabase::importFileBufferToDb(skipDuplicates);

}

QString Browser::generatePatchText(int index, const char* name)
{
	index++;

	QString text = QString::number(index);

	if (index < 10) text += ".   ";
	else if (index < 100) text += ".  ";
	else text += ". ";

	text += name;

	return text;
}

void Browser::setProgressBarCount(int count)
{
	setDisabled(true);
	

	ui.progressBar->setMaximum(count);
	ui.progressBar->setValue(0);
}

void Browser::incrementProgressBar()
{
	auto value = ui.progressBar->value() + 1;
	ui.progressBar->setValue(value);

	if (value != ui.progressBar->maximum()) return;

	ui.progressBar->setValue(0);

	setDisabled(false);

}

void Browser::setPatchesToTableView(const std::vector<PatchRow>& patches)
{
	model.setData(patches);
}

void Browser::scrollToBottom()
{
	int maximum = ui.databaseView->verticalScrollBar()->maximum();

	ui.databaseView->verticalScrollBar()->setSliderPosition(maximum);
}
