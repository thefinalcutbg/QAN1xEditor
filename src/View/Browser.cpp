#include "Browser.h"
#include "Model/PatchMemory.h"
#include "Model/PatchDatabase.h"
#include <algorithm>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <qdebug.h>


Browser::Browser(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	column_sort.setDynamicSortFilter(true);

	column_sort.setSourceModel(&model);
	search.setSourceModel(&column_sort);
	search.setFilterKeyColumn(1);
	ui.databaseView->setModel(&search);

	ui.databaseView->hideColumn(0);

	ui.databaseView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui.databaseView->setSelectionBehavior(QAbstractItemView::SelectRows);

	ui.databaseView->horizontalHeader()->setHighlightSections(false);

	ui.databaseView->verticalHeader()->setVisible(false);

	ui.databaseView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.databaseView->viewport()->setFocusPolicy(Qt::StrongFocus);

	ui.databaseView->setShowGrid(true);


	ui.databaseView->verticalHeader()->setDefaultSectionSize(20);
	ui.databaseView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	ui.databaseView->horizontalHeader()->setStretchLastSection(true);

	ui.databaseView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	connect(ui.searchTypeCombo, &QComboBox::currentIndexChanged, [&](int index) {

		search.setFilterKeyColumn(index + 1);

	});

	connect(ui.lineEdit, &QLineEdit::textChanged, [=]
	{
			QString text = ui.lineEdit->text();
			search.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::PatternOption::CaseInsensitiveOption));

	});

	connect(ui.databaseView->horizontalHeader(), &QHeaderView::sectionClicked, [&](int column) {

		column_sort.sort(column);

	});

	connect(ui.deleteButton, &QPushButton::clicked, [&]{

		auto idxList = ui.databaseView->selectionModel()->selectedRows();

		std::set<long long>selectedRowids;

		for (auto& idx : idxList) {
			long long index = search.index(idx.row(), 0).data().toLongLong();
			selectedRowids.insert(index);
		}

		if (selectedRowids.empty()) return;

		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "Warning", "Delete is permanent. Are you sure?",
			QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::No) {
			return;
		}

		PatchDatabase::deleteSelectedPatches(selectedRowids);

	});

	for (int i = 0; i < 128; i++)
	{
		ui.An1xList->addItem(generatePatchText(i, "InitNormal"));
	}

	connect(ui.loadButton, &QPushButton::clicked, [&] {PatchMemory::loadFromAn1x(getSelectedIndexes());});
	connect(ui.sendButton, &QPushButton::clicked, [&] {PatchMemory::sendToAn1x(getSelectedIndexes()); });
	connect(ui.An1xList, &QListWidget::doubleClicked, [&] {

		auto indexes = getSelectedIndexes();

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


	PatchMemory::setBrowserView(this);
	PatchDatabase::setBrowserView(this);

}

void Browser::setPatchName(int idx, const std::string& name)
{
	ui.An1xList->item(idx)->setText(generatePatchText(idx, name.c_str()));
}

Browser::~Browser()
{}

std::vector<int> Browser::getSelectedIndexes()
{

	QModelIndexList indexes = ui.An1xList->selectionModel()->selectedIndexes();

	std::vector<int> indexList;

	for (QModelIndex& index : indexes){
		indexList.push_back(index.row());
	}
	
	return indexList;
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
			PatchDatabase::loadAn1File(std::vector<unsigned char>{bytes.begin(), bytes.end()}, fileInfo.fileName().toStdString());;
		}
		catch (std::exception) {
			QMessageBox msgBox;
			msgBox.setText("The file is corrupted");
			msgBox.exec();
		}

		file.close();
	}
}

QString Browser::generatePatchText(int index, const char* name)
{
	index++;

	QString text = QString::number(index);

	if (index < 10) text += ".     ";
	else if (index < 100) text += ".   ";
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
