#include "QMidiAn1x.h"
#include "qmidimessage.h"
#include "GlobalWidgets.h"
#include "Model/MidiMaster.h"

QMidiAn1x::QMidiAn1x(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    GlobalWidgets::statusBar = statusBar();

    m.setView(this);

    ui.lowFq->setValueTextType(DialKnob::Frequency);
    ui.midFq->setValueTextType(DialKnob::Frequency);
    ui.highFq->setValueTextType(DialKnob::Frequency);
    ui.midRes->setValueTextType(DialKnob::Resonance);
    ui.lowGain->setValueTextType(DialKnob::Gain);
    ui.midGain->setValueTextType(DialKnob::Gain);
    ui.highGain->setValueTextType(DialKnob::Gain);

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
    
    connect(ui.variFxType, &QComboBox::currentIndexChanged, [=](int index) { setFxLayout(index); });
    connect(ui.delayType, &QComboBox::currentIndexChanged, [=](int index) { setFxLayout(index); });
    connect(ui.reverbType, &QComboBox::currentIndexChanged, [=](int index) { setFxLayout(index); });

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
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        ui.variFxType,
        nullptr,
        ui.fxParam1,
        nullptr,
        ui.fxParam2,
        nullptr,
        ui.fxParam3,
        nullptr,
        ui.fxParam4,
        nullptr,
        ui.fxParam5,
        nullptr,
        ui.fxParam6,
        nullptr,
        ui.lowFq,
        ui.lowGain,
        ui.midFq,
        ui.midGain,
        ui.midRes,
        ui.highFq,
        ui.highGain,
        ui.DlyRevConnection, //serial
        ui.delayType,
        ui.dlyReturn,
        ui.dlyParam1,
        nullptr,
        ui.dlyParam2,
        nullptr,
        ui.dlyParam3,
        nullptr,
        ui.dlyParam4,
        nullptr,
        ui.dlyParam5,
        nullptr,
        ui.dlyParam6,
        nullptr,
        ui.dlyParam7,
        nullptr,
        ui.reverbType,
        ui.revReturn,
        ui.revParam1,
        nullptr,
        ui.revParam2,
        nullptr,
        ui.revParam3,
        nullptr,
        ui.revParam4,
        nullptr,
        ui.revParam5,
        nullptr,
        ui.revParam6,
        nullptr,
        ui.revParam7,
        nullptr
    };

    for (int i = 0; i < 80/*AN1x::SceneParam::SceneParamSize*/; i++)
    {
        if (ui_controls[i] == nullptr) continue;

        ui_controls[i]->setCurrentValueAsDefault();
        ui_controls[i]->setCommonParam(&m, (AN1x::CommonParam)i);
    }

    ui.scene2tab->setAsScene2();

    ui.scene1tab->setMidiMaster(&m);
    ui.scene2tab->setMidiMaster(&m);

    m.refreshConnection();


}

void QMidiAn1x::setMidiDevices(const QStringList& in, const QStringList& out)
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



void QMidiAn1x::setSceneParameter(AN1x::SceneParam p, int value, bool isScene2)
{
    auto& sceneView = isScene2 ? *ui.scene2tab : *ui.scene1tab;

    sceneView.setSceneParameters(p, value);
}

void QMidiAn1x::setCommonParameter(AN1x::CommonParam p, int value)
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

    if (p == AN1x::SceneSelect) {
       
        QRadioButton* group[3]{ ui.scene1radio, ui.scene2radio, ui.bothSceneRadio };
        group[value]->setChecked(true);

    }

    if (p < AN1x::CommonParamSize && ui_controls[p] != nullptr) {
        ui_controls[p]->setValue(value);
    }

    if (p == AN1x::VariFXType)  setFxLayout(value);
    
}

void QMidiAn1x::setFxLayout(int value)
{

    QSignalBlocker b[] = {
        QSignalBlocker{ui.fxParam1},
        QSignalBlocker{ui.fxParam2},
        QSignalBlocker{ui.fxParam3},
        QSignalBlocker{ui.fxParam4},
        QSignalBlocker{ui.fxParam5},
        QSignalBlocker{ui.fxParam6}
    };

    switch ((AN1x::VariFx)value)
    {
        case AN1x::Chorus1:
        case AN1x::Chorus2:
        {
            ui.fxLabel1->setText("Freqency");
            ui.fxLabel2->setText("PM Depth");
            ui.fxLabel3->setText("Amplitude");
            ui.fxLabel4->setText("Delay Offset");
            ui.fxLabel5->clear();
            ui.fxLabel6->clear();

            ui.fxParam4->show();
            ui.fxParam5->hide();
            ui.fxParam6->hide();
        
            ui.fxParam1->setRange(0, 159);
            ui.fxParam1->setValue(2);
            ui.fxParam1->setCurrentValueAsDefault();
            ui.fxParam1->setValueTextType(DialKnob::LFOFrquency);

            ui.fxParam2->setRange(0, 100);
            ui.fxParam2->setValue(2);
            ui.fxParam2->setCurrentValueAsDefault();
            ui.fxParam2->setValueTextType(DialKnob::Percent);

            ui.fxParam3->setRange(0, 15);
            ui.fxParam3->setValue(14);
            ui.fxParam3->setCurrentValueAsDefault();
            ui.fxParam3->setValueTextType(DialKnob::ChorusType);

            ui.fxParam4->setRange(0, 500);
            ui.fxParam4->setValue(194);
            ui.fxParam4->setCurrentValueAsDefault();
            ui.fxParam4->setValueTextType(DialKnob::Milliseconds);
        }
        break;

        case AN1x::Flagner:
        {
            ui.fxLabel1->setText("Freqency");
            ui.fxLabel2->setText("Mod Depth");
            ui.fxLabel3->setText("Delay Offset");
            ui.fxLabel4->setText("Feedback Level");
            ui.fxLabel5->clear();
            ui.fxLabel6->clear();

            ui.fxParam4->show();
            ui.fxParam5->hide();
            ui.fxParam6->hide();

            ui.fxParam1->setRange(0, 159);
            ui.fxParam1->setValue(2);
            ui.fxParam1->setCurrentValueAsDefault();
            ui.fxParam1->setValueTextType(DialKnob::LFOFrquency);

            ui.fxParam2->setRange(0, 100);
            ui.fxParam2->setValue(2);
            ui.fxParam2->setCurrentValueAsDefault();
            ui.fxParam2->setValueTextType(DialKnob::Percent);

            ui.fxParam3->setRange(0, 155);
            ui.fxParam3->setValue(14);
            ui.fxParam3->setCurrentValueAsDefault();
            ui.fxParam3->setValueTextType(DialKnob::Milliseconds);

            ui.fxParam4->setRange(0, 198);
            ui.fxParam4->setValue(100);
            ui.fxParam4->setCurrentValueAsDefault();
            ui.fxParam4->setValueTextType(DialKnob::Milliseconds);
        }
        break;

    }
}

void QMidiAn1x::setDelayLayout(int value)
{
}

void QMidiAn1x::setReverbLayout(int value)
{
}

QMidiAn1x::~QMidiAn1x()
{}
