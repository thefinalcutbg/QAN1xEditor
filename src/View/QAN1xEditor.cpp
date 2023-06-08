#include "QAN1xEditor.h"
#include "qmidimessage.h"
#include "GlobalWidgets.h"
#include "Model/MidiMaster.h"

QAN1xEditor::QAN1xEditor(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);


    GlobalWidgets::statusBar = statusBar();

    m.setView(this);



    for (int y = 0; y < 6; y++)
    {
        ui.paramMtrxCom_1->addItem(AN1x::getMatrixCommonParamName(y));
        ui.paramMtrxCom_2->addItem(AN1x::getMatrixCommonParamName(y));
    }

    ui.depthMtrxCom_1->showPlusOnPositives(true);
    ui.depthMtrxCom_2->showPlusOnPositives(true);


    //COMMON MATRIX CONTROLS
    for (int y = 0; y < 115; y++)
    {
        ui.sourceMtrxCom_1->addItem(AN1x::getMatrixSourceName(y).c_str());
        ui.sourceMtrxCom_2->addItem(AN1x::getMatrixSourceName(y).c_str());
    }


    //MIDI DEVICES
    connect(ui.refresh, &QPushButton::clicked, [&] { m.refreshConnection(); });
    connect(ui.inCombo, &QComboBox::currentIndexChanged, [&](int index) { m.connectMidiIn(index - 1); });
    connect(ui.outCombo, &QComboBox::currentIndexChanged, [&](int index) { m.connectMidiOut(index - 1); });

    //LAYER
    connect(ui.single, &QRadioButton::clicked, [&] { m.setCommonParam(AN1x::LayerMode, !ui.unison->isChecked() ? AN1x::Single : AN1x::Unison); });
    connect(ui.dual, &QRadioButton::clicked, [&] { m.setCommonParam(AN1x::LayerMode, !ui.unison->isChecked() ? AN1x::Dual : AN1x::DualUnison); });
    connect(ui.split, &QRadioButton::clicked, [&] { m.setCommonParam(AN1x::LayerMode, !ui.unison->isChecked() ? AN1x::Split : AN1x::SplitUnison); });
    connect(ui.unison, &QGroupBox::clicked, [&](bool checked) {

        if (ui.single->isChecked())
        {
            m.setCommonParam(AN1x::LayerMode, !checked ? AN1x::Single : AN1x::Unison);
        }
        else if (ui.dual->isChecked())
        {
            m.setCommonParam(AN1x::LayerMode, !checked ? AN1x::Dual : AN1x::Unison);
        }
        else if (ui.split->isChecked())
        {
            m.setCommonParam(AN1x::LayerMode, !checked ? AN1x::Split : AN1x::SplitUnison);
        }
        }
    );

    connect(ui.scene1radio, &QRadioButton::clicked, [&] { m.setCommonParam(AN1x::SceneSelect, 0); });
    connect(ui.scene2radio, &QRadioButton::clicked, [&] { m.setCommonParam(AN1x::SceneSelect, 1); });
    connect(ui.bothSceneRadio, &QRadioButton::clicked, [&] { m.setCommonParam(AN1x::SceneSelect, 2); });
    connect(ui.tempoBPM, &QSpinBox::valueChanged, [&](int value) { m.setCommonParam(AN1x::CommonParam::Tempo, value); });
    connect(ui.midiSyncCheck, &QCheckBox::clicked, [&](bool checked) { 
            ui.tempoBPM->setDisabled(checked);
            m.setCommonParam(AN1x::CommonParam::Tempo, checked ? 39 : ui.tempoBPM->value());
        }
    );
    


    ui.ctrlMatrixScene2->setAsScene2();


    ui.voiceNameEdit->setMidiMaster(&m);

    ui_controls = {
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        ui.voiceType,
        nullptr,
        nullptr,
        ui.layerPan,
        ui.separation,
        ui.detune,
        nullptr,
        nullptr,
        nullptr,
        ui.portamentoCheck,
        ui.sourceMtrxCom_1,
        ui.paramMtrxCom_1,
        ui.depthMtrxCom_1,
        ui.sourceMtrxCom_2,
        ui.paramMtrxCom_2,
        ui.depthMtrxCom_2
        
    };

    for (int i = 0; i < ui_controls.size(); i++)
    {
        if (ui_controls[i] == nullptr) continue;

        ui_controls[i]->setCurrentValueAsDefault();
        ui_controls[i]->setCommonParam(&m, (AN1x::CommonParam)i);
    }

    ui.arpSeqCheck->setCurrentValueAsDefault();
    ui.arpSeqCheck->setCommonParam(&m, AN1x::ArpSeqOnOff);

    ui.scene2tab->setAsScene2();

    ui.scene1tab->setMidiMaster(&m);
    ui.scene2tab->setMidiMaster(&m);

    ui.ctrlMatrixScene1->setMidiMaster(&m);
    ui.ctrlMatrixScene2->setMidiMaster(&m);

    ui.seqTab->setMidiMaster(&m);

    ui.fxeqTab->setMidiMaster(&m);

    m.refreshConnection();


}

void QAN1xEditor::setMidiDevices(const QStringList& in, const QStringList& out)
{
    {
        QSignalBlocker b1(ui.inCombo);
        QSignalBlocker b2(ui.outCombo);

        ui.inCombo->clear();
        ui.outCombo->clear();

        ui.outCombo->addItem("None");
        ui.inCombo->addItem("None");

        for (auto& string : in) ui.inCombo->addItem(string);
        for (auto& string : out) ui.outCombo->addItem(string);
    }

    ui.inCombo->setCurrentIndex(ui.inCombo->count() - 1);
    ui.outCombo->setCurrentIndex(ui.outCombo->count() - 1);

}



void QAN1xEditor::setSceneParameter(AN1x::SceneParam p, int value, bool isScene2)
{


    auto& sceneView = isScene2 ? *ui.scene2tab : *ui.scene1tab;

    sceneView.setSceneParameters(p, value);
    
    auto& ctrlMatrix = isScene2 ? *ui.ctrlMatrixScene1 : *ui.ctrlMatrixScene2;

    ctrlMatrix.setSceneParameters(p, value);

    ui.fxeqTab->setSceneParameter(p, value, isScene2);
}

void QAN1xEditor::setCommonParameter(AN1x::CommonParam p, int value)
{
    //name edit
    if (p < 10) {
        ui.voiceNameEdit->setName(p, value);
        return;
    }



    //layer mode
    if (p == AN1x::LayerMode)
    {
        switch ((AN1x::Layer)value) {
            case AN1x::Single:
                ui.unison->setChecked(false);
                ui.single->setChecked(true);
                break;
            case AN1x::Unison:
                ui.unison->setChecked(true);
                ui.single->setChecked(true);
                break;
            case AN1x::Dual:
                ui.unison->setChecked(false);
                ui.dual->setChecked(true);
                break;
            case AN1x::DualUnison:
                ui.unison->setChecked(true);
                ui.dual->setChecked(true);
                ui.split->setChecked(false);
                break;
            case AN1x::Split:
                ui.unison->setChecked(false);
                ui.split->setChecked(true);
                break;
            case AN1x::SplitUnison:
                ui.unison->setChecked(false);
                ui.split->setChecked(true);
                break;
        }
        
        return;
    }

    if (p == AN1x::Tempo) {

        if (value == 39) {
            ui.midiSyncCheck->setChecked(true);
        }
        ui.midiSyncCheck->setChecked(false);
        ui.tempoBPM->setValue(value);

        return;
    }

    if (p == AN1x::ArpSeqOnOff) {
        ui.arpSeqCheck->setValue(value);
    }

    if (p == AN1x::SceneSelect) {
       
        QRadioButton* group[3]{ ui.scene1radio, ui.scene2radio, ui.bothSceneRadio };
        group[value]->setChecked(true);

    }

    if (p < AN1x::VariFXType && ui_controls[p] != nullptr) {
        ui_controls[p]->setValue(value);
    }

    ui.fxeqTab->setCommonParameter(p, value);
    ui.seqTab->setCommonParameter(p, value);
}

void QAN1xEditor::setSequenceParameter(AN1x::SeqParam p, int value)
{
    ui.seqTab->setSequenceParameter(p, value);
}

QAN1xEditor::~QAN1xEditor()
{}