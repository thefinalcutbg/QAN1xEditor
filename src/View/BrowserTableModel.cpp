#include "BrowserTableModel.h"
#include <QIcon>


QIcon getTypeIcon(int type) {

    if (type < 1 || type > 22) return QIcon();

    QString qrcPath = ":/icon/resources/icon";
    qrcPath += QString::number(type);
    qrcPath += ".png";
     
    return QIcon(qrcPath);
}

QString getTypeText(int type) {
    
    if (type < 1 || type > 22) type = 0;

     QString arr[] = {
        "", 
        "Piano", 
        "Chromatic Percussion", 
        "Organ", 
        "Guitar", 
        "Bass", 
        "Strings", 
        "Ensemble", 
        "Brass", 
        "Reed", 
        "Pipe", 
        "Synth Lead", 
        "Synth Pad", 
        "Synth Effect",
        "Ethnic", 
        "Percussion",
        "Sound Effect",
        "Drums", 
        "Synth Comp", 
        "Vocal", 
        "Combination", 
        "Material Wave", 
        "Sequence"
    };

    return arr[type];
}


void BrowserTableModel::setData(const std::vector<PatchRow>& rows)
{
    beginResetModel();

    list = rows;

    endResetModel();

    emit dataChanged(index(0, 0), index(list.size(), columnCount()));
}

QVariant BrowserTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
                case 0: return "Rowid";
                case 1: return "Name";
                case 2: return "Type";
                case 3: return "AN1 file";
                case 4: return "Created by";
                case 5: return "Song";
                case 6: return "Artist";
                case 7: return "Comment";
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

    switch (role)
    {
        
        case Qt::DecorationRole:
        {
            switch (column)
            {
            case 1:
                    return getTypeIcon(list[row].type);

                break;
            default:
                return QVariant();
            }
        }
        case Qt::DisplayRole:
        {
            switch (column)
            {
                case 0: return list[row].rowid;
                case 1: return list[row].name;
                case 2: return getTypeText(list[row].type);
                case 3: return list[row].file;
                case 4: return list[row].created_by;
                case 5: return list[row].song;
                case 6: return list[row].artist;
                case 7: return list[row].comment;
                default: return QVariant();
            }
            
        }
        case Qt::TextAlignmentRole: return int(Qt::AlignLeft | Qt::AlignVCenter);
    }

    return QVariant();

}
