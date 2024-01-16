#pragma once

#include <QAbstractTableModel>
#include "Model/An1xPatch.h"

#include <vector>

class BrowserTableModel : public QAbstractTableModel
{
	Q_OBJECT

		struct PatchRow {
		int index = 0;
		int type = 0;
		QString name;
		QString song;
		QString artist;
		QString comment;
	};

	std::vector<PatchRow> list;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
	BrowserTableModel() {}
	void setData(const std::vector<AN1xPatch> patches);
	int rowCount(const QModelIndex& parent = QModelIndex()) const override { return list.size(); }
	int columnCount(const QModelIndex& parent = QModelIndex()) const override { return 6; }

};
