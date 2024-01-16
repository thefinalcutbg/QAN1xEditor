#include "BrowserTableModel.h"


void BrowserTableModel::setData(const std::vector<AN1xPatch> patches)
{
    beginResetModel();

    list.clear();
    list.reserve(patches.size());

    for (int i = 0; i < patches.size(); i++) {
        list.emplace_back();
        list.back().index = i;
        list.back().type = patches[i].getType();
        list.back().name = patches[i].getName().c_str();
        list.back().song = patches[i].metadata.song.c_str();
        list.back().artist = patches[i].metadata.artist.c_str();
        list.back().comment = patches[i].metadata.comment.c_str();
    }

    endResetModel();

    emit dataChanged(index(0, 0), index(list.size(), 4));
}

QVariant BrowserTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
                case 0: return "Index";
                case 1: return "Type";
                case 2: return "Name";
                case 3: return "Song";
                case 4: return "Artist";
                case 5: return "Comment";
            }
        }
    }

    return QVariant();;
}

QVariant BrowserTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();

    int row = index.row();
    int column = index.column();

    //if (row == m_procedures.size()) return 0; //why???
    //if (m_procedures.size() == 0) return 0;


    switch (role)
    {
        /*
        case Qt::DecorationRole:
        {
            switch (column)
            {
            case 5:
                if (m_procedures[row].fsource == FinancingSource::NHIF)
                    return QIcon(":/icons/icon_nhif.png");
                if (m_procedures[row].fsource == FinancingSource::PHIF)
                    return QIcon(":/icons/icon_phif.png");
                break;
            case 1:
                switch (m_procedures[row].db_source)
                {
                case Procedure::Local: return QIcon(":/icons/icon_db.png");
                case Procedure::PIS: return QIcon(":/icons/icon_nhif.png");
                case Procedure::HIS:  return QIcon(":/icons/icon_his.png");
                }
                break;
            default:
                return QVariant();
            }
        }
        */

        case Qt::DisplayRole:
        {
            switch (column)
            {
                case 0: return list[row].index+1;
                case 1: return list[row].type;
                case 2: return list[row].name;
                case 3: return list[row].song;
                case 4: return list[row].artist;
                case 5: return list[row].comment;
                default: break;
            }
            
        }
        case Qt::TextAlignmentRole: return int(Qt::AlignLeft | Qt::AlignVCenter);
    }

    return QVariant();

}
