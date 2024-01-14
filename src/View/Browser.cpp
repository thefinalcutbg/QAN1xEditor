#include "Browser.h"
#include "Model/PatchMemory.h"
#include "Model/PatchMemory.h"
#include <algorithm>

Browser::Browser(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	for (int i = 0; i < 128; i++)
	{
		ui.An1xList->addItem(generatePatchText(i, "InitNormal"));
	}

	connect(ui.loadButton, &QPushButton::clicked, [&] {PatchMemory::loadFromAn1x(getSelectedIndexes());});

	connect(ui.An1xList, &QListWidget::doubleClicked, [&] {

		auto indexes = getSelectedIndexes();

		if (indexes.empty()) return;

		PatchMemory::loadAn1xMemPatch(indexes[0]);

	});

	PatchMemory::setBrowser(this);

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
