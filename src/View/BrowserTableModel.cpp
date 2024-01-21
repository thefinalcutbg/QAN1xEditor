#include "BrowserTableModel.h"
#include "FreeFunctions.h"



QString getTypeText(int type) {
    
    if (type < 1 || type > 22) type = 0;

     static const QString arr[] = {
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

QString getLayerText(int type){

    if (type < 0 || type > 5) type = 6;

    static const QString arr[] = {
        "Single","Unison","Dual","Dual Unison","Split","Split Unison",""
    };

    return arr[type];
}

QString getEffectText(int type) {

    if (type < 0 || type > 14) type = 14;

    static const QString arr[] = {
      "Chorus 1", "Chorus 2", "Flagner", "Symphonic", "Phaser", "Auto PAN", "Rotary Speaker", "Pitch Change", "Aural Exciter", "Compressor", "Wah Wah", "Distortion", "Overdrive", "Amp Simulator", ""
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
                case 0: return "RowID";
                case 1: return "TypeID";
                case 2: return "LayerID";
                case 3: return "EffectID";
                case 4: return "ArpSeqID";
                case 5: return "Name";
                case 6: return "Type";
                case 7: return "Layer";
                case 8: return "Effect";
                case 9: return "Arp/Seq";
                case 10: return "AN1 file";
                case 11: return "Comment";
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
            case 5:
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
                case 2: return list[row].layer;
                case 3: return list[row].effect;
                case 4: return list[row].arp_seq;
                case 5: return list[row].name;
                case 6: return getTypeText(list[row].type);
                case 7: return getLayerText(list[row].layer);
                case 8: return getEffectText(list[row].effect);
                case 9: return list[row].arp_seq ? "Yes" : "No";
                case 10: return list[row].file;
                case 11: return list[row].comment;
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

