#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QAN1xEditor.h"
#include "qmidiin.h"
#include "qmidiout.h"
#include "Model/MidiMaster.h"

class QAN1xEditor : public QMainWindow
{
    Q_OBJECT

    MidiMaster m;

    std::array<AbstractSceneController*, AN1x::CommonParam::VariFXType> ui_controls{ nullptr };

public:
    QAN1xEditor(QWidget *parent = nullptr);
    void setMidiDevices(const QStringList& in, const QStringList& out);
    void setSceneParameter(AN1x::SceneParam p, int value, bool isScene2);
    void setCommonParameter(AN1x::CommonParam p, int value);
    void setSequenceParameter(AN1x::SeqParam p, int value);
    ~QAN1xEditor();

private:
    Ui::QAN1xEditor ui;
};
