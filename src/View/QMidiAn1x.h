#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QMidiAn1x.h"
#include "qmidiin.h"
#include "qmidiout.h"
#include "Model/MidiMaster.h"

class QMidiAn1x : public QMainWindow
{
    Q_OBJECT

    MidiMaster m;

    std::array<AbstractSceneController*, AN1x::CommonParam::VariFXType> ui_controls{ nullptr };

public:
    QMidiAn1x(QWidget *parent = nullptr);
    void setMidiDevices(const QStringList& in, const QStringList& out);
    void setSceneParameter(AN1x::SceneParam p, int value, bool isScene2);
    void setCommonParameter(AN1x::CommonParam p, int value);
    ~QMidiAn1x();

private:
    Ui::QMidiAn1xClass ui;
};
