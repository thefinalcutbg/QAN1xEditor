#include "BrowserTableModel.h"
#include "FreeFunctions.h"



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
                case 1: return "Typeid";
                case 2: return "Name";
                case 3: return "Type";
                case 4: return "AN1 file";
                case 5: return "Created by";
                case 6: return "Song";
                case 7: return "Artist";
                case 8: return "Comment";
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
            case 2:
                    return FreeFn::getTypeIcon(list[row].type);

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
                case 1: return list[row].type;
                case 2: return list[row].name;
                case 3: return getTypeText(list[row].type);
                case 4: return list[row].file;
                case 5: return list[row].created_by;
                case 6: return list[row].song;
                case 7: return list[row].artist;
                case 8: return list[row].comment;
                default: return QVariant();
            }
            
        }
        case Qt::TextAlignmentRole: return int(Qt::AlignLeft | Qt::AlignVCenter);
    }

    return QVariant();

}

Qt::ItemFlags BrowserTableModel::flags(const QModelIndex& index) const
{
    auto flags = Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    return flags;
}

Qt::DropActions BrowserTableModel::supportedDropActions() const
{
    auto flags = Qt::DropActions();
    flags |= Qt::MoveAction;
    return flags;
}

