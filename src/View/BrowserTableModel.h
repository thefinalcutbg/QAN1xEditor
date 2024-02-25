#pragma once

#include <QAbstractTableModel>
#include "Model/PatchRow.h"
#include <vector>

class BrowserTableModel : public QAbstractTableModel
{
	Q_OBJECT

	std::vector<PatchRow> list;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    Qt::DropActions supportedDropActions() const override;


public:
	BrowserTableModel() {}
    void setPatchData(const std::vector<PatchRow>& rows);
    int rowCount(const QModelIndex& = QModelIndex()) const override { return list.size(); }
    int columnCount(const QModelIndex& = QModelIndex()) const override { return 12; }

};
