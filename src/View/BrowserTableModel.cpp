#include "BrowserTableModel.h"
#include "FreeFunctions.h"
#include <QEvent>
#include <QMouseEvent>

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
      "Chorus 1", "Chorus 2", "Flanger", "Symphonic", "Phaser", "Auto PAN", "Rotary Speaker", "Pitch Change", "Aural Exciter", "Compressor", "Wah Wah", "Distortion", "Overdrive", "Amp Simulator", ""
    };

    return arr[type];
}

void BrowserTableModel::setPatchData(const std::vector<PatchRow>& rows)
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
                case 0: return "rowID";
                case 1: return "TypeID";
                case 2: return "LayerID";
                case 3: return "EffectID";
                case 4: return "ArpSeqID";
                case 5: return "";
                case 6: return "Name";
                case 7: return "Type";
                case 8: return "Layer";
                case 9: return "Effect";
                case 10: return "Arp/Seq";
                case 11: return "AN1 file";
                case 12: return "Comment";
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
            case 6:
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
                case 5: return !list[row].fav;  //the boolean is reversed, to enable sorting!
                case 6: return list[row].name;
                case 7: return getTypeText(list[row].type);
                case 8: return getLayerText(list[row].layer);
                case 9: return getEffectText(list[row].effect);
                case 10: return list[row].arp_seq ? "Yes" : "No";
                case 11: return list[row].file;
                case 12: return list[row].comment;
                default: return QVariant();
            }
            
        }
        case Qt::TextAlignmentRole: return int(Qt::AlignLeft | Qt::AlignVCenter);
    }

    return QVariant();

}

Qt::ItemFlags BrowserTableModel::flags(const QModelIndex&) const
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


bool FavButtonDelegate::mouseIsOnStar(QMouseEvent* e, const QStyleOptionViewItem& option)
{
    auto pos = e->position();

    int clickX = pos.x();
    int clickY = pos.y();

    QRect r = option.rect;//getting the rect of the cell
    int x, y, w, h;
    x = r.left()+5;//the X coordinate
    y = r.top()+5;//the Y coordinate
    w = 20;//button width
    h = 20;//button height

    if (clickX > x && clickX < x + w)
        if (clickY > y && clickY < y + h)
        {
            return true;
        }

    return false;
}

FavButtonDelegate::FavButtonDelegate(QObject* parent)
    : QItemDelegate(parent)
{

}
#include <QPainter>
void FavButtonDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    //the boolean is reversed, to enable sorting!
    auto isFavourite = !index.data(Qt::DisplayRole).toBool();

    QRect r = option.rect;//getting the rect of the cell
    int x, y, w, h;
    x = r.left()+5;//the X coordinate
    y = r.top()+2;//the Y coordinate
    w = 20;//button width
    h = 20;//button height
    auto starRect = QRect(x, y, w, h);

    const QIcon* px = nullptr;

    if (isFavourite) {
        px = &star_yellow;
    }
    else {
        px = &star_gray;
    }

    if (!px) return;

    px->paint(painter, starRect);

    if (m_row_hover == index.row() && m_row_hover != -1)
    {
        star_hover.paint(painter, starRect);
    }

}


bool FavButtonDelegate::editorEvent(QEvent* event, QAbstractItemModel*, const QStyleOptionViewItem& option, const QModelIndex& index)
{


    if (event->type() == QEvent::MouseMove) {

        m_row_hover = mouseIsOnStar((QMouseEvent*)event, option) ? index.row() : -1;

        emit updateRequested();

        return true;
    }

    if (event->type() == QEvent::MouseButtonPress)
    {
        if (!mouseIsOnStar((QMouseEvent*)event, option)) return false;

        emit favouriteClicked(index.row());

        return false;

    }

    return false;
}
