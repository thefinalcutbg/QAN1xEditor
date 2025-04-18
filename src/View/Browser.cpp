#include "Browser.h"

//#include <algorithm>

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QScrollBar>
#include <QInputDialog>
#include <QPlainTextEdit>

#include "FreeFunctions.h"
#include "GlobalWidgets.h"

#include "Database/Database.h"

#include "Model/DragDropManager.h"
#include "Model/PatchMemory.h"
#include "Model/PatchDatabase.h"
#include "Model/ClipoboardManager.h"
#include "Model/An1File.h"

Browser::Browser(QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    GlobalWidgets::browser = this;

    column_sort.setDynamicSortFilter(true);

    column_sort.setSourceModel(&model);
    search.setSourceModel(&column_sort);
    search.setFilterKeyColumn(6);
    ui.databaseView->setModel(&search);
    ui.databaseView->setMouseTracking(true);
    disableWidgets(false);

    auto favDelagete = new FavButtonDelegate();
    ui.databaseView->setItemDelegateForColumn(5, favDelagete);

    ui.databaseView->horizontalHeader()->setMinimumSectionSize(30);

    connect(favDelagete, &FavButtonDelegate::updateRequested, this, [&] {
        ui.databaseView->viewport()->update();
    });

    connect(favDelagete, &FavButtonDelegate::favouriteClicked, this, [&](int row) {

        PatchDatabase::setFavourite(
            search.index(row, 5).data().toBool(),
            search.index(row, 0).data().toLongLong()
        );

    });

    for (int i = 0; i < 5; i++) {
        ui.databaseView->hideColumn(i);
    }

    ui.databaseView->setColumnWidth(5, 30);
    ui.databaseView->setColumnWidth(10, 55);

    connect(ui.searchTypeCombo, &QComboBox::currentIndexChanged, this, [&](int index) {

        search.setFilterKeyColumn(index + 6);
        refreshCountLabel();

    });

    connect(ui.An1xList->model(), &QAbstractListModel::rowsMoved, this, [&](const QModelIndex&, int start, int, const QModelIndex& , int row)
            {
                PatchMemory::rowMoved(start, row);

                recalculateListNames();
            });

    connect(ui.commentButton, &QPushButton::clicked, this, [&] { editComment(); });

    connect(ui.lineEdit, &QLineEdit::textChanged, this, [=]
            {
                QString text = ui.lineEdit->text();
                search.setFilterRegularExpression(QRegularExpression(text, QRegularExpression::PatternOption::CaseInsensitiveOption));
                refreshCountLabel();

            });

    connect(ui.databaseView->horizontalHeader(), &QHeaderView::sectionClicked, this, [&](int column) {

        const int hiddenColumnMap[] = { 0,1,2,3,4,5,1,2,3,4,10,11,12 };

        column_sort.sort(hiddenColumnMap[column]);

    });

    connect(ui.deleteButton, &QPushButton::clicked, this, [&] {

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

    connect(ui.databaseView, &DbTableView::deletePressed, this, [&] { ui.deleteButton->click(); });

    connect(ui.An1xList, &MemoryList::deleteRequested, this, [&] { PatchMemory::initPatches(getSelectedListIndexes()); });

    connect(ui.loadAN1ToList, &QPushButton::clicked, this, [&] { loadAN1FileToList();  });

    connect(ui.saveAN1file, &QPushButton::clicked, this, [&] { saveAN1File(); });

    connect(ui.exportAN2Button, &QPushButton::clicked, this, [&] { exportAN2File();  });

    connect(ui.cancelButton, &QPushButton::clicked, this, [&] {

        PatchMemory::loadFromAn1x({});
        PatchMemory::sendToAn1x({});
        disableWidgets(false);

        ui.progressBar->setValue(0);


    });

    connect(ui.databaseView, &DbTableView::copyRequested, this, [&] {
        ClipboardManager::copyRequestFromDatabase(getSelectedTableRowids());
    });

    connect(ui.An1xList, &MemoryList::copyRequested, this, [&] {
        ClipboardManager::copyRequestFromMemoryList(getSelectedListIndexes());
    });

    connect(ui.An1xList, &MemoryList::pasteRequested, this, [&]{

        auto indexes = getSelectedListIndexes();

        if (indexes.empty()) return;

        ClipboardManager::pasteToListRequested(indexes[0]);

    });

    for (int i = 0; i < 128; i++)
    {
        ui.An1xList->addItem("");
        setPatchToListView(i, "InitNormal", 0);
    }

    connect(ui.loadButton, &QPushButton::clicked, this, [&] {PatchMemory::loadFromAn1x(getSelectedListIndexes());});
    connect(ui.sendButton, &QPushButton::clicked, this, [&] {PatchMemory::sendToAn1x(getSelectedListIndexes()); });
    connect(ui.An1xList, &QListWidget::doubleClicked, this, [&] {

        auto indexes = getSelectedListIndexes();

        if (indexes.empty()) return;

        PatchMemory::loadAn1xMemPatch(indexes[0]);

    });

    connect(ui.importAn1, &QPushButton::clicked, this, [&] {
        importAN1FileButtonClicked();
    });

    connect(ui.importAn2, &QPushButton::clicked, this, [&] {
        importAN2FileButtonClicked();
    });

    connect(ui.databaseView, &QTableView::doubleClicked, this, [&](const QModelIndex& index) {

        int idx = search.index(index.row(), 0).data().toLongLong();
        PatchDatabase::setVoiceAsCurrent(idx);

    });

    connect(ui.databaseView, &DbTableView::dataDroped, this, [&] {
        DragDropManager::droppedToDbTable(getSelectedListIndexes());

    });

    connect(ui.An1xList, &MemoryList::dataDroped, this, [&](int row) {
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

    PatchDatabase::importFileBufferToDb();

}

void Browser::importAN2FileButtonClicked()
{
    auto fileName = QFileDialog::getOpenFileName(this,
                                                 tr("Open QAN1xEditor"), QDir::homePath(), "QAn1xEditor file(*.qan1)");

    if (fileName.isEmpty()) return;

    PatchDatabase::importExternalDb(fileName.toStdString());
}

void Browser::loadAN1FileToList()
{
    auto fileName = QFileDialog::getOpenFileName(this,
                                                 tr("Open AN1xEdit file"), QDir::homePath(), "AN1xEdit file(*.an1)");

    if (fileName.isEmpty()) return;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) return;

    auto bytes = file.readAll();

    try {
        PatchMemory::loadAn1File({ std::vector<unsigned char>{bytes.begin(), bytes.end()}, "" });
    }
    catch (std::exception) {
        QMessageBox msgBox;
        msgBox.setText("The file is corrupted");
        msgBox.exec();
    }

    file.close();
}


void Browser::exportAN2File()
{

    auto fileName = QFileDialog::getSaveFileName(this,
                                                 tr("Export QAN1xEditor"), QDir::homePath() + "/patches.qan1", "QAn1xEditor file(*.qan1)");

    if (fileName.isEmpty()) return;

    QFile::copy(Db::getDbPath().c_str(), fileName);
}

void Browser::editComment()
{
    std::set<long long> rowids = getSelectedTableRowids();

    if (rowids.empty()) return;

    QString comment;

    if (rowids.size() == 1) {

        comment = search.index(ui.databaseView->selectionModel()->selectedIndexes().first().row(), 12).data().toString();
    }
    else {

        QMessageBox::information(this, qAppName(), "Changes will affect multiple patches!");
    }

    QInputDialog d;
    d.setInputMode(QInputDialog::InputMode::TextInput);
    d.setOption(QInputDialog::InputDialogOption::UsePlainTextEditForTextInput);

    auto textEdit = d.findChild<QPlainTextEdit*>();

    textEdit->setLineWrapMode(QPlainTextEdit::LineWrapMode::WidgetWidth);
    textEdit->setWordWrapMode(QTextOption::WrapMode::WordWrap);

    d.setLabelText("Comment:");
    d.setTextValue(comment);

    if (d.exec() != QDialog::Accepted) return;

    PatchDatabase::updateComment(d.textValue().toStdString(), rowids);
}

void Browser::disableWidgets(bool disabled)
{
	ui.progressSpacer->changeSize(0, 0, disabled ? QSizePolicy::Fixed : QSizePolicy::Expanding);

	for (auto obj : children()) {
		if (obj->isWidgetType()) {
			static_cast<QWidget*>(obj)->setDisabled(disabled);
		}
	}
	ui.progressBar->setTextVisible(disabled);
	ui.progressBar->setHidden(!disabled);
	ui.cancelButton->setHidden(!disabled);
	ui.cancelButton->setDisabled(!disabled);

}

void Browser::saveAN1File()
{
    auto fileName = QFileDialog::getSaveFileName(this,
        tr("Save as An1xEdit file"), QDir::homePath() + "/AN1xEdit1.an1", "An1xEdit file(*.an1)");

    if (fileName.isEmpty()) return;

    QFile f(fileName);

    f.open(QIODeviceBase::WriteOnly);

    auto source = PatchMemory::getFile();

    QByteArray byteArray(reinterpret_cast<const char*>(source.data()), source.size());

    f.write(byteArray);
    
    f.close();
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
    disableWidgets(count);
    ui.progressBar->setMaximum(count);
    ui.progressBar->setValue(0);
}

void Browser::incrementProgressBar()
{
    auto value = ui.progressBar->value() + 1;
    ui.progressBar->setValue(value);

    if (value != ui.progressBar->maximum()) return;

    ui.progressBar->setValue(0);

    disableWidgets(false);

}

void Browser::setPatchesToTableView(const std::vector<PatchRow>& patches)
{
    model.setPatchData(patches);
    refreshCountLabel();
}

void Browser::scrollToBottom()
{
    int maximum = ui.databaseView->verticalScrollBar()->maximum();

    ui.databaseView->verticalScrollBar()->setSliderPosition(maximum);
}

void Browser::refreshCountLabel()
{
    ui.countLabel->setText(QString("Total results: ") + QString::number(search.rowCount()));
}
