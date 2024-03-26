#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QAN1xEditor.h"

class AN1xPatch;

class QAN1xEditor : public QMainWindow
{
    Q_OBJECT

    bool eventFilter(QObject* obj, QEvent* event);

    std::array<AbstractController*, AN1x::CommonParam::VariFXType> ui_controls{ nullptr };
    std::array<AbstractController*, AN1x::SystemParam::SystemMaxSize> system_controls{ nullptr };

    void setSystemParameter(AN1x::SystemParam p, int value);
    void setSceneParameter(AN1x::SceneParam p, int value, bool isScene2, bool setAsDefault = false);
    void setCommonParameter(AN1x::CommonParam p, int value, bool setAsDefault = false);
    void setSequenceParameter(AN1x::SeqParam p, int value, bool setAsDefault = false);
    void initializeInitMenu();
    void setBypass();
    unsigned char layerMode();

public:
    QAN1xEditor(QWidget *parent = nullptr);
    void setPatch(const AN1xPatch& patch);
    void setMidiDevices(const QStringList& in, const QStringList& out);
    void setParameter(ParamType type, unsigned char param, int value, bool setAsDefault = false);
    void setModWheel(int value);
    void setTrackData(const std::vector<int>& trackData);
    Browser* browser();
    PianoView* pianoRoll();

    //two byte values are represented by int and 0 and has to be separated;

    ~QAN1xEditor();

private:
    Ui::QAN1xEditor ui;
};
