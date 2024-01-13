#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QAN1xEditor.h"
#include "qmidiin.h"
#include "qmidiout.h"

class An1xPatch;

class QAN1xEditor : public QMainWindow
{
    Q_OBJECT

    bool eventFilter(QObject* obj, QEvent* event);

    std::array<AbstractController*, AN1x::CommonParam::VariFXType> ui_controls{ nullptr };
    std::array<AbstractController*, AN1x::SystemParam::SystemMaxSize> system_controls{ nullptr };

    void setSystemParameter(AN1x::SystemParam p, int value);
    void setSceneParameter(AN1x::SceneParam p, int value, bool isScene2);
    void setCommonParameter(AN1x::CommonParam p, int value);
    void setSequenceParameter(AN1x::SeqParam p, int value);

    unsigned char layerMode();

public:
    QAN1xEditor(QWidget *parent = nullptr);
    void setPatch(const An1xPatch& patch);
    void setMidiDevices(const QStringList& in, const QStringList& out);
    void setParameter(AN1x::ParamType type, unsigned char param, int value);
    void setModWheel(int value);
    void setTrackData(const std::vector<int>& trackData);
    PianoView* pianoRoll();

    //two byte values are represented by int and 0 and has to be separated;
    std::vector<int> getCommon();

    ~QAN1xEditor();

private:
    Ui::QAN1xEditor ui;
};
