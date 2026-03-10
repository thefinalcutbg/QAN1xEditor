#include "TemplateDialog.h"

#include <QListWidgetItem>
#include <QMessageBox>
#include "Model/PatchDatabase.h"

TemplateDialog::TemplateDialog(int type, QWidget* parent)
	: QDialog(parent)
	, m_type(type)
{
	ui.setupUi(this);

	setWindowTitle("Templates");

	ui.loadButton->setEnabled(false);
	ui.deleteButton->setEnabled(false);

	refreshList();

	connect(ui.listWidget, &QListWidget::itemSelectionChanged, this, [this]() {
		const bool hasSelection = ui.listWidget->currentItem() != nullptr;
		ui.loadButton->setEnabled(hasSelection);
		ui.deleteButton->setEnabled(hasSelection);
	});

	connect(ui.loadButton, &QPushButton::clicked, this, [this]() {
		loadCurrent();
	});

	connect(ui.deleteButton, &QPushButton::clicked, this, [this]() {
		deleteCurrent();
	});

	connect(ui.listWidget, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem*) {
		loadCurrent();
	});
}

TemplateDialog::~TemplateDialog()
{
}

std::vector<unsigned char> TemplateDialog::getResult() const
{
	if (!m_loadPressed)
		return {};

	return m_result;
}

void TemplateDialog::refreshList()
{
	ui.listWidget->clear();

	auto list = PatchDatabase::getTemplateList(m_type);

	for (const auto& [rowid, name] : list) {
		auto* item = new QListWidgetItem(QString::fromStdString(name));
		item->setData(Qt::UserRole, QVariant::fromValue<qlonglong>(rowid));
		ui.listWidget->addItem(item);
	}

	bool hasSelection = ui.listWidget->currentItem() != nullptr;
	ui.loadButton->setEnabled(hasSelection);
	ui.deleteButton->setEnabled(hasSelection);
}

void TemplateDialog::loadCurrent()
{
	auto* item = ui.listWidget->currentItem();
	if (!item)
		return;

	long long rowid = item->data(Qt::UserRole).toLongLong();

	m_result = PatchDatabase::getTemplateData(rowid);
	m_loadPressed = true;
	accept();
}

void TemplateDialog::deleteCurrent()
{
	auto* item = ui.listWidget->currentItem();

	if (!item) return;

	const auto answer = QMessageBox::question(
		this,
		"Delete template",
		QString("Are you sure you want to delete template?"),
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No
	);

	if (answer != QMessageBox::Yes)
		return;

	long long rowid = item->data(Qt::UserRole).toLongLong();
	PatchDatabase::removeTemplate(rowid);

	refreshList();
}