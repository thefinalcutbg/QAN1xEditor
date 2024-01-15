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

		try {
			PatchDatabase::loadAn1File(std::vector<unsigned char>{bytes.begin(), bytes.end()});;
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
