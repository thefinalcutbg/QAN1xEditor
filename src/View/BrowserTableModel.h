#pragma once

#include <QAbstractTableModel>
#include "Model/PatchRow.h"
#include <vector>
#include <QIcon>
#include <QItemDelegate>

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
    int columnCount(const QModelIndex& = QModelIndex()) const override { return 13; }

};

class QMouseEvent;

class FavButtonDelegate : public QItemDelegate
{
    Q_OBJECT

    QIcon star_yellow{ ":/icon/resources/icon_fav.png" };
    QIcon star_gray{ ":/icon/resources/icon_favgray.png" };
    QIcon star_hover{ ":/icon/resources/icon_favhover.png" };

    int m_row_hover = -1;

    bool mouseIsOnStar(QMouseEvent* e, const QStyleOptionViewItem& option);

public:
    FavButtonDelegate(QObject* parent = 0);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);

signals:
    void favouriteClicked(int row);
    void updateRequested();
};
