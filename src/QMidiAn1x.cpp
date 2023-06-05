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
    ui.lowGain->setOffset(-64);
    ui.lowGain->setSuffix("dB");
    ui.lowGain->setShowPositive(true);
    ui.midGain->setOffset(-64);
    ui.midGain->setSuffix("dB");
    ui.midGain->setShowPositive(true);
    ui.highGain->setOffset(-64);
    ui.highGain->setSuffix("dB");
    ui.highGain->setShowPositive(true);

    setFxLayout(0);

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

    
    ui.fxParam1->resetValueText();
    ui.fxParam1->setNotchesVisible(false);
    ui.fxParam2->resetValueText();
    ui.fxParam2->setNotchesVisible(false);
    ui.fxParam3->resetValueText();
    ui.fxParam3->setNotchesVisible(false);
    ui.fxParam4->resetValueText();
    ui.fxParam4->setNotchesVisible(false);
    ui.fxParam5->resetValueText();
    ui.fxParam5->setNotchesVisible(false);
    ui.fxParam6->resetValueText();
    ui.fxParam6->setNotchesVisible(false);

    switch ((AN1x::VariFx)value)
    {
        case AN1x::Chorus1:
        case AN1x::Chorus2:
        {
            ui.fxLabel1->setText("Freqency");
            ui.fxParam1->setRange(0, 159);
            ui.fxParam1->setValue(2);
            ui.fxParam1->setValueTextType(DialKnob::LFOFrquency);

            ui.fxLabel2->setText("PM Depth");
            ui.fxParam2->setRange(0, 100);
            ui.fxParam2->setValue(2);
            ui.fxParam2->setSuffix("%");

            ui.fxLabel3->setText("Amplitude");
            ui.fxParam3->setRange(0, 15);
            ui.fxParam3->setValue(14);
            ui.fxParam3->setValueTextType(DialKnob::ChorusType);
            ui.fxParam3->setNotchTarget(1);
            ui.fxParam3->setNotchesVisible(true);

            ui.fxLabel4->setText("Delay Offset");
            ui.fxParam4->show();
            ui.fxParam4->setRange(0, 500);
            ui.fxParam4->setValue(194);
            ui.fxParam4->setValueTextType(DialKnob::Milliseconds);

            ui.fxLabel5->setText("");
            ui.fxParam5->hide();

            ui.fxLabel6->setText("");
            ui.fxParam6->hide();
        }
        break;

        case AN1x::Flagner:
        {
            ui.fxLabel1->setText("Freqency");
            ui.fxParam1->setRange(0, 159);
            ui.fxParam1->setValue(6);
            ui.fxParam1->setValueTextType(DialKnob::LFOFrquency);

            ui.fxLabel2->setText("Mod Depth");
            ui.fxParam2->setRange(0, 100);
            ui.fxParam2->setValue(75);
            ui.fxParam2->setSuffix("%");

            ui.fxLabel3->setText("Delay Offset");
            ui.fxParam3->setRange(0, 155);
            ui.fxParam3->setValue(19);
            ui.fxParam3->setValueTextType(DialKnob::Milliseconds);

            ui.fxLabel4->setText("Feedback Level");
            ui.fxParam4->show();
            ui.fxParam4->setRange(0, 198);
            ui.fxParam4->setValue(167);
            ui.fxParam4->setOffset(-99);

            ui.fxLabel5->setText("");
            ui.fxParam5->hide();

            ui.fxLabel6->setText("");
            ui.fxParam6->hide();
        }
        break;

        case AN1x::Symphonic:
        {
            ui.fxLabel1->setText("Freqency");
            ui.fxParam1->setRange(0, 159);
            ui.fxParam1->setValue(3);
            ui.fxParam1->setValueTextType(DialKnob::LFOFrquency);

            ui.fxLabel2->setText("Mod Depth");
            ui.fxParam2->setRange(0, 100);
            ui.fxParam2->setValue(70);
            ui.fxParam2->setSuffix("%");

            ui.fxLabel3->setText("Delay Offset");
            ui.fxParam3->setRange(0, 450);
            ui.fxParam3->setValue(189);
            ui.fxParam3->setValueTextType(DialKnob::Milliseconds);

            ui.fxLabel4->setText("");
            ui.fxParam4->hide();

            ui.fxLabel5->setText("");
            ui.fxParam5->hide();

            ui.fxLabel6->setText("");
            ui.fxParam6->hide();
        }
        break;

        case AN1x::Phaser:
        {
            ui.fxLabel1->setText("Freqency");
            ui.fxParam1->setRange(0, 5);
            ui.fxParam1->setValue(5);
            ui.fxParam1->setValueTextType(DialKnob::LFOFrquency);

            ui.fxLabel2->setText("Mod Depth");
            ui.fxParam2->setRange(0, 100);
            ui.fxParam2->setValue(60);
            ui.fxParam2->setSuffix("%");

            ui.fxLabel3->setText("Phaser Offset");
            ui.fxParam3->setRange(0, 100);
            ui.fxParam3->setValue(39);

            ui.fxLabel4->setText("Feedback Level");
            ui.fxParam4->show();
            ui.fxParam4->setRange(0, 198);
            ui.fxParam4->setValue(176);
            ui.fxParam4->setOffset(-99);

            ui.fxLabel5->setText("Stage");
            ui.fxParam5->show();
            ui.fxParam5->setRange(0, 2);
            ui.fxParam5->setValue(0);
            ui.fxParam5->setValueTextType(DialKnob::Stage);
            ui.fxParam5->setNotchesVisible(true);

            ui.fxLabel6->setText("Diffusion");
            ui.fxParam6->show();
            ui.fxParam6->setRange(0,1);
            ui.fxParam6->setValue(1);
            ui.fxParam6->setValueTextType(DialKnob::Diffusion);
            ui.fxParam6->setNotchesVisible(true);
        }
        break;

        case AN1x::AutoPAN:
        {
            ui.fxLabel1->setText("Speed");
            ui.fxParam1->setRange(0, 159);
            ui.fxParam1->setValue(27);
            ui.fxParam1->setValueTextType(DialKnob::LFOFrquency);

            ui.fxLabel2->setText("Mod Depth");
            ui.fxParam2->setRange(0, 100);
            ui.fxParam2->setValue(75);

            ui.fxLabel3->setText("Delay Offset");
            ui.fxParam3->setRange(0, 100);
            ui.fxParam3->setValue(0);

            ui.fxLabel4->setText("PAN Direction");
            ui.fxParam4->show();
            ui.fxParam4->setRange(0, 4);
            ui.fxParam4->setValue(3);
            ui.fxParam4->setValueTextType(DialKnob::PanDirection);
            ui.fxParam4->setNotchesVisible(true);

            ui.fxLabel5->setText("");
            ui.fxParam5->hide();

            ui.fxLabel6->setText("");
            ui.fxParam6->hide();
        }
        break;

        case AN1x::RotarySpeaker:
        {
            ui.fxLabel1->setText("Speed");
            ui.fxParam1->setRange(0, 159);
            ui.fxParam1->setValue(41);
            ui.fxParam1->setValueTextType(DialKnob::LFOFrquency);

            ui.fxLabel2->setText("Depth");
            ui.fxParam2->setRange(0, 100);
            ui.fxParam2->setValue(100);
            ui.fxParam2->setSuffix("%");

            ui.fxLabel3->setText("HPF");
            ui.fxParam3->setRange(0, 52);
            ui.fxParam3->setValue(0);
            ui.fxParam3->setValueTextType(DialKnob::Frequency);

            ui.fxLabel4->setText("LPF");
            ui.fxParam4->show();
            ui.fxParam4->setRange(34, 60);
            ui.fxParam4->setValue(60);
            ui.fxParam4->setValueTextType(DialKnob::Frequency);

            ui.fxLabel5->setText("");
            ui.fxParam5->hide();

            ui.fxLabel6->setText("");
            ui.fxParam6->hide();
        }
        break;

        case AN1x::PitchChange:
        {
            
            ui.fxLabel1->setText("Pitch");
            ui.fxParam1->setRange(0, 48);
            ui.fxParam1->setValue(24);
            ui.fxParam1->setOffset(-24);
            ui.fxParam1->setShowPositive(true);

            ui.fxLabel2->setText("Fine 1");
            ui.fxParam2->setRange(0, 100);
            ui.fxParam2->setValue(53);
            ui.fxParam2->setOffset(-50);

            ui.fxLabel3->setText("Pan 1");
            ui.fxParam3->setRange(1, 127);
            ui.fxParam3->setValue(1);
            ui.fxParam3->setOffset(-64);
            ui.fxParam3->setShowPositive(true);

            ui.fxLabel4->setText("Fine 2");
            ui.fxParam4->show();
            ui.fxParam4->setRange(0, 100);
            ui.fxParam4->setValue(47);
            ui.fxParam4->setOffset(-50);;

            ui.fxLabel5->setText("Pan 2");
            ui.fxParam5->show();
            ui.fxParam5->setRange(1, 127);
            ui.fxParam5->setValue(127);
            ui.fxParam5->setOffset(-64);
            ui.fxParam5->setShowPositive(true);

            ui.fxLabel6->setText("");
            ui.fxParam6->hide();
        }
        break;

        case AN1x::AuralExciter:
        {
            ui.fxLabel1->setText("HPF");
            ui.fxParam1->setRange(28, 58);
            ui.fxParam1->setValue(32);
            ui.fxParam1->setValueTextType(DialKnob::Frequency);

            ui.fxLabel2->setText("Drive");
            ui.fxParam2->setRange(0, 100);
            ui.fxParam2->setValue(80);
            ui.fxParam2->setSuffix("%");

            ui.fxLabel3->setText("Mix Level");
            ui.fxParam3->setRange(0, 100);
            ui.fxParam3->setValue(40);

            ui.fxLabel4->setText("");
            ui.fxParam4->hide();

            ui.fxLabel5->setText("");
            ui.fxParam5->hide();

            ui.fxLabel6->setText("");
            ui.fxParam6->hide();
        }
        break;

        case AN1x::Compressor:
        {
            ui.fxLabel1->setText("Attack");
            ui.fxParam1->setRange(0, 19);
            ui.fxParam1->setValue(4);
            ui.fxParam1->setValueTextType(DialKnob::CompressorAttack);

            ui.fxLabel2->setText("Release");
            ui.fxParam2->setRange(0, 15);
            ui.fxParam2->setValue(0);
            ui.fxParam2->setValueTextType(DialKnob::CompressorRelease);

            ui.fxLabel3->setText("Threshold");
            ui.fxParam3->setRange(0, 42);
            ui.fxParam3->setValue(13);
            ui.fxParam3->setOffset(-48);
            ui.fxParam3->setSuffix("dB");

            ui.fxLabel4->setText("Ratio");
            ui.fxParam4->show();
            ui.fxParam4->setRange(0, 7);
            ui.fxParam4->setValue(5);
            ui.fxParam4->setValueTextType(DialKnob::CompressorRatio);

            ui.fxLabel5->setText("Out Level");
            ui.fxParam5->show();
            ui.fxParam5->setRange(1, 100);
            ui.fxParam5->setValue(90);
            
            ui.fxLabel6->setText("");
            ui.fxParam6->hide();
        }
        break;

        case AN1x::WahWah:
        {
            ui.fxLabel1->setText("Sensitivity");
            ui.fxParam1->setRange(0, 100);
            ui.fxParam1->setValue(30);

            ui.fxLabel2->setText("Cutoff Frq Offset");
            ui.fxParam2->setRange(0, 39);
            ui.fxParam2->setValue(5);
            ui.fxParam2->setValueTextType(DialKnob::WahCutoffFreq);

            ui.fxLabel3->setText("Resonance");
            ui.fxParam3->setRange(0, 90);
            ui.fxParam3->setValue(59);
            ui.fxParam3->setOffset(10);
            ui.fxParam3->setValueTextType(DialKnob::Resonance);

            ui.fxLabel4->setText("");
            ui.fxParam4->hide();

            ui.fxLabel5->setText("");
            ui.fxParam5->hide();

            ui.fxLabel6->setText("");
            ui.fxParam6->hide();
        }
        break;

        case AN1x::Distortion:
        case AN1x::Overdrive:
        {
            ui.fxLabel1->setText("Drive");
            ui.fxParam1->setRange(0, 100);
            ui.fxParam1->setValue(100);

            ui.fxLabel2->setText("Mid Freq");
            ui.fxParam2->setRange(14, 54);
            ui.fxParam2->setValue(33);
            ui.fxParam2->setValueTextType(DialKnob::Frequency);

            ui.fxLabel3->setText("Mid Gain");
            ui.fxParam3->setRange(52, 76);
            ui.fxParam3->setOffset(-64);
            ui.fxParam3->setSuffix("dB");
            ui.fxParam3->setValue(67);
            ui.fxParam3->setShowPositive(true);

            ui.fxLabel4->setText("High Freq");
            ui.fxParam4->show();
            ui.fxParam4->setRange(28, 58);
            ui.fxParam4->setValue(56);
            ui.fxParam4->setValueTextType(DialKnob::Frequency);

            ui.fxLabel5->setText("High Gain");
            ui.fxParam5->show();
            ui.fxParam5->setRange(52, 76);
            ui.fxParam5->setOffset(-64);
            ui.fxParam5->setSuffix("dB");
            ui.fxParam5->setValue(68);
            ui.fxParam5->setShowPositive(true);

            ui.fxLabel6->setText("Out Level");
            ui.fxParam6->show();
            ui.fxParam6->setRange(0,100);
            ui.fxParam6->setValue(75);

        }
        break;

        case AN1x::AmpSimulator:
        {
            ui.fxLabel1->setText("Drive");
            ui.fxParam1->setRange(1, 100);
            ui.fxParam1->setValue(100);

            ui.fxLabel2->setText("Amp Type");
            ui.fxParam2->setRange(0, 3);
            ui.fxParam2->setNotchesVisible(true);
            ui.fxParam2->setValue(1);
            ui.fxParam2->setValueTextType(DialKnob::AMPType);

            ui.fxLabel3->setText("LPF");
            ui.fxParam3->setRange(34, 60);
            ui.fxParam3->setValue(48);
            ui.fxParam3->setValueTextType(DialKnob::Frequency);

            ui.fxLabel4->setText("Out Level");
            ui.fxParam4->show();
            ui.fxParam4->setRange(0, 100);
            ui.fxParam4->setValue(60);

            ui.fxLabel5->setText("");
            ui.fxParam5->hide();

            ui.fxLabel6->setText("");
            ui.fxParam6->hide();
        }
        break;
    }

    ui.fxParam1->setCurrentValueAsDefault();
    ui.fxParam2->setCurrentValueAsDefault();
    ui.fxParam3->setCurrentValueAsDefault();
    ui.fxParam4->setCurrentValueAsDefault();
    ui.fxParam5->setCurrentValueAsDefault();
    ui.fxParam6->setCurrentValueAsDefault();
}

void QMidiAn1x::setDelayLayout(int value)
{
}

void QMidiAn1x::setReverbLayout(int value)
{
}

QMidiAn1x::~QMidiAn1x()
{}
