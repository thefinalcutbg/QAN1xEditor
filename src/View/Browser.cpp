#include "Browser.h"

#include <algorithm>

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
    search.setFilterKeyColumn(5);
    ui.databaseView->setModel(&search);

    disableWidgets(false);

    for (int i = 0; i < 5; i++) {
        ui.databaseView->hideColumn(i);
    }

    ui.databaseView->setColumnWidth(9, 55);


    connect(ui.searchTypeCombo, &QComboBox::currentIndexChanged, [&](int index) {

        search.setFilterKeyColumn(index + 5);

    });

    connect(ui.An1xList->model(), &QAbstractListModel::rowsMoved, [&](const QModelIndex& parent, int start, int end, const QModelIndex& destination, int row)
            {
                PatchMemory::rowMoved(start, row);

                recalculateListNames();
            });

    connect(ui.commentButton, &QPushButton::clicked, [&] { editComment(); });

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

    connect(ui.An1xList, &MemoryList::deleteRequested, [&] { PatchMemory::initPatches(getSelectedListIndexes()); });

    connect(ui.loadAN1ToList, &QPushButton::clicked, [&] { loadAN1FileToList();  });

    connect(ui.initButton, &QPushButton::clicked, [&] { PatchMemory::initPatches(getSelectedListIndexes()); });

    connect(ui.exportAN2Button, &QPushButton::clicked, [&] { exportAN2File();  });

    connect(ui.cancelButton, &QPushButton::clicked, [&] {

        PatchMemory::loadFromAn1x({});

        disableWidgets(false);

        ui.progressBar->setValue(0);


    });

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

    connect(ui.importAn1, &QPushButton::clicked, [&] {
        importAN1FileButtonClicked();
    });

    connect(ui.importAn2, &QPushButton::clicked, [&] {
        importAN2FileButtonClicked();
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
                                                 tr("Open QAN1xEditor"), QDir::homePath(), "QAn1xEditor file(*.an2)");

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

    QFileInfo fileInfo(file.fileName());

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
                                                 tr("Export QAN1xEditor"), QDir::homePath() + "patches.an2", "QAn1xEditor file(*.an2)");

    if (fileName.isEmpty()) return;

    QFile::copy(Db::getDbPath().c_str(), fileName);
}

void Browser::editComment()
{
    std::set<long long> rowids = getSelectedTableRowids();

    if (rowids.empty()) return;

    QString comment;

    if (rowids.size() == 1) {

        comment = search.index(ui.databaseView->selectionModel()->selectedIndexes().first().row(), 11).data().toString();
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
    ui.groupBox->setDisabled(disabled);
    ui.groupBox_2->setDisabled(disabled);

    ui.progressBar->setHidden(!disabled);
    ui.cancelButton->setHidden(!disabled);
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
    model.setData(patches);
}

void Browser::scrollToBottom()
{
    int maximum = ui.databaseView->verticalScrollBar()->maximum();

    ui.databaseView->verticalScrollBar()->setSliderPosition(maximum);
}
