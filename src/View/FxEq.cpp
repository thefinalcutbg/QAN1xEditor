#include "FxEq.h"


FxEq::FxEq(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);


    connect(ui.variFxType, &QComboBox::currentIndexChanged, [&](int index) { setFxLayout(index); });
    connect(ui.delayType, &QComboBox::currentIndexChanged, [&](int index) { setDelayLayout(index); });
    connect(ui.dlyBypass, &QCheckBox::clicked, [&] { setBypass(); });
    connect(ui.revBypass, &QCheckBox::clicked, [&] { setBypass(); });
    connect(ui.allBypass, &QCheckBox::clicked, [&] { setBypass(); });

    setFxLayout(0);
    setDelayLayout(0);
    setReverbLayout();
    setEqLayout();
     
    ui_controls = {
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
        ui.revParam7
    };

    ui.scene1DW->setParam(ParamType::Scene1, AN1x::SceneParam::VariFxDW);
    ui.scene2DW->setParam(ParamType::Scene2, AN1x::SceneParam::VariFxDW);

    ui.scene1DW->showPlusOnPositives(true);
    ui.scene2DW->showPlusOnPositives(true);

    for (int i = 0; i < ui_controls.size(); i++)
    {
        if (ui_controls[i] == nullptr) continue;

        auto param = AN1x::VariFXType + i;

        ui_controls[i]->setCurrentValueAsDefault();
        ui_controls[i]->setParam(ParamType::Common, (AN1x::CommonParam)param);
    }


}

void FxEq::setSystemParameter(AN1x::SystemParam p, int value)
{
    if (p != AN1x::SystemParam::EffectBypass) return;

    QSignalBlocker all(ui.allBypass);
    QSignalBlocker dly(ui.dlyBypass);
    QSignalBlocker rev(ui.revBypass);

    switch (value)
    {
    case 0: 
        ui.allBypass->setChecked(false);
        ui.dlyBypass->setChecked(false);
        ui.revBypass->setChecked(false);
        break;
    case 1:
        ui.allBypass->setChecked(false);
        ui.dlyBypass->setChecked(true);
        ui.revBypass->setChecked(false);
        break;
    case 2:
        ui.allBypass->setChecked(false);
        ui.dlyBypass->setChecked(false);
        ui.revBypass->setChecked(true);
        break;
    case 3:
        ui.allBypass->setChecked(false);
        ui.dlyBypass->setChecked(true);
        ui.revBypass->setChecked(true);
        break;
    case 4:
        ui.allBypass->setChecked(true);
        ui.dlyBypass->setChecked(true);
        ui.revBypass->setChecked(true);
        break;
    }
}

void FxEq::setCommonParameter(AN1x::CommonParam p, int value)
{
    int idx = p - AN1x::VariFXType;

    if (idx >= 0 && idx < ui_controls.size() &&  ui_controls[idx] != nullptr) {
        ui_controls[idx]->setValue(value);
    }

    if (p == AN1x::VariFXType)setFxLayout(value);

    if (p == AN1x::DlyType) setDelayLayout(value);
}

void FxEq::setSceneParameter(AN1x::SceneParam p, int value, bool isScene2)
{
    if (p != AN1x::VariFxDW) return;

    auto& dial = isScene2 ? ui.scene1DW : ui.scene2DW;
    dial->setValue(value);

}

void FxEq::setFxLayout(int value)
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

    ui.scene1DW->setOffset(-64);
    ui.scene1DW->setNotchTarget(70);
    ui.scene2DW->setOffset(-64);
    ui.scene2DW->setNotchTarget(70);
    ui.dryWetLabel1->setText("D=W");
    ui.dryWetLabel2->setText("D=W");

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
        ui.fxParam2->setValue(78);
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

        ui.fxLabel4->setText("FB Level");
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

        ui.fxLabel4->setText("FB Level");
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
        ui.fxParam6->setRange(0, 1);
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
        ui.fxParam3->setValueTextType(DialKnob::EqFrequency);

        ui.fxLabel4->setText("LPF");
        ui.fxParam4->show();
        ui.fxParam4->setRange(34, 60);
        ui.fxParam4->setValue(60);
        ui.fxParam4->setValueTextType(DialKnob::EqFrequency);

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
        ui.fxParam1->showPlusOnPositives(true);

        ui.fxLabel2->setText("Fine 1");
        ui.fxParam2->setRange(0, 100);
        ui.fxParam2->setValue(53);
        ui.fxParam2->setOffset(-50);
        ui.fxParam2->showPlusOnPositives(true);

        ui.fxLabel3->setText("Pan 1");
        ui.fxParam3->setRange(1, 127);
        ui.fxParam3->setValue(1);
        ui.fxParam3->setOffset(-64);
        ui.fxParam3->showPlusOnPositives(true);

        ui.fxLabel4->setText("Fine 2");
        ui.fxParam4->show();
        ui.fxParam4->setRange(0, 100);
        ui.fxParam4->setValue(47);
        ui.fxParam4->setOffset(-50);
        ui.fxParam4->showPlusOnPositives(true);

        ui.fxLabel5->setText("Pan 2");
        ui.fxParam5->show();
        ui.fxParam5->setRange(1, 127);
        ui.fxParam5->setValue(127);
        ui.fxParam5->setOffset(-64);
        ui.fxParam5->showPlusOnPositives(true);

        ui.fxLabel6->setText("");
        ui.fxParam6->hide();
    }
    break;

    case AN1x::AuralExciter:
    {
        ui.fxLabel1->setText("HPF");
        ui.fxParam1->setRange(28, 58);
        ui.fxParam1->setValue(32);
        ui.fxParam1->setValueTextType(DialKnob::EqFrequency);

        ui.fxLabel2->setText("Drive");
        ui.fxParam2->setRange(0, 100);
        ui.fxParam2->setValue(80);

        ui.fxLabel3->setText("Mix Level");
        ui.fxParam3->setRange(0, 100);
        ui.fxParam3->setValue(40);

        ui.fxLabel4->setText("");
        ui.fxParam4->hide();

        ui.fxLabel5->setText("");
        ui.fxParam5->hide();

        ui.fxLabel6->setText("");
        ui.fxParam6->hide();


        ui.scene1DW->setNotchTarget(127);
        ui.scene2DW->setNotchTarget(127);
        ui.dryWetLabel1->setText("");
        ui.dryWetLabel2->setText("");

    }
    break;

    case AN1x::Compressor:
    {
        ui.fxLabel1->setText("Attack");
        ui.fxParam1->setRange(0, 19);
        ui.fxParam1->setValue(2);
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

        ui.scene1DW->setNotchTarget(127);
        ui.scene2DW->setNotchTarget(127);
        ui.dryWetLabel1->setText("");
        ui.dryWetLabel2->setText("");
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

        ui.scene1DW->setNotchTarget(70);
        ui.scene2DW->setNotchTarget(70);
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
        ui.fxParam2->setValueTextType(DialKnob::EqFrequency);

        ui.fxLabel3->setText("Mid Gain");
        ui.fxParam3->setRange(52, 76);
        ui.fxParam3->setOffset(-64);
        ui.fxParam3->setSuffix("dB");
        ui.fxParam3->setValue(67);
        ui.fxParam3->showPlusOnPositives(true);

        ui.fxLabel4->setText("High Freq");
        ui.fxParam4->show();
        ui.fxParam4->setRange(28, 58);
        ui.fxParam4->setValue(56);
        ui.fxParam4->setValueTextType(DialKnob::EqFrequency);

        ui.fxLabel5->setText("High Gain");
        ui.fxParam5->show();
        ui.fxParam5->setRange(52, 76);
        ui.fxParam5->setOffset(-64);
        ui.fxParam5->setSuffix("dB");
        ui.fxParam5->setValue(68);
        ui.fxParam5->showPlusOnPositives(true);

        ui.fxLabel6->setText("Out Level");
        ui.fxParam6->show();
        ui.fxParam6->setRange(0, 100);
        ui.fxParam6->setValue(75);

        ui.scene1DW->setNotchTarget(70);
        ui.scene2DW->setNotchTarget(70);
        ui.dryWetLabel1->setText("Both");
        ui.dryWetLabel2->setText("Both");
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
        ui.fxParam3->setValueTextType(DialKnob::EqFrequency);

        ui.fxLabel4->setText("Out Level");
        ui.fxParam4->show();
        ui.fxParam4->setRange(0, 100);
        ui.fxParam4->setValue(60);

        ui.fxLabel5->setText("");
        ui.fxParam5->hide();

        ui.fxLabel6->setText("");
        ui.fxParam6->hide();

        ui.scene1DW->setNotchTarget(70);
        ui.scene2DW->setNotchTarget(70);
        ui.dryWetLabel1->setText("Both");
        ui.dryWetLabel2->setText("Both");
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

void FxEq::setDelayLayout(int value)
{

    QSignalBlocker b[] = {
        QSignalBlocker{ui.dlyParam1},
        QSignalBlocker{ui.dlyParam2},
        QSignalBlocker{ui.dlyParam3},
        QSignalBlocker{ui.dlyParam4},
        QSignalBlocker{ui.dlyParam5},
        QSignalBlocker{ui.dlyParam6},
        QSignalBlocker{ui.dlyParam7}
    };


    ui.dlyParam1->resetValueText();
    ui.dlyParam1->setNotchesVisible(false);
    ui.dlyParam2->resetValueText();
    ui.dlyParam2->setNotchesVisible(false);
    ui.dlyParam3->resetValueText();
    ui.dlyParam3->setNotchesVisible(false);
    ui.dlyParam4->resetValueText();
    ui.dlyParam4->setNotchesVisible(false);
    ui.dlyParam5->resetValueText();
    ui.dlyParam5->setNotchesVisible(false);
    ui.dlyParam6->resetValueText();
    ui.dlyParam6->setNotchesVisible(false);
    ui.dlyParam7->resetValueText();
    ui.dlyParam7->setNotchesVisible(false);


    switch ((AN1x::Delay)value)
    {
    case AN1x::LCR:

        ui.dlyLabel1->setText("Left");
        ui.dlyParam1->setRange(0, 6599);
        ui.dlyParam1->setOffset(1);
        ui.dlyParam1->setValue(453);
        ui.dlyParam1->setValueTextType(DialKnob::Milliseconds);

        ui.dlyLabel2->setText("Right");
        ui.dlyParam2->setRange(0, 6599);
        ui.dlyParam2->setOffset(1);
        ui.dlyParam2->setValue(715);
        ui.dlyParam2->setValueTextType(DialKnob::Milliseconds);

        ui.dlyLabel3->setText("Center");
        ui.dlyParam3->setRange(0, 6599);
        ui.dlyParam3->setOffset(1);
        ui.dlyParam3->setValue(3999);
        ui.dlyParam3->setValueTextType(DialKnob::Milliseconds);

        ui.dlyLabel4->setText("Center lvl.");
        ui.dlyParam4->setRange(0, 100);
        ui.dlyParam4->setValue(90);

        ui.dlyLabel5->setText("FB level");
        ui.dlyParam5->setRange(0, 198);
        ui.dlyParam5->setValue(131);
        ui.dlyParam5->setOffset(-99);
        ui.dlyParam5->setSuffix("%");

        ui.dlyLabel6->setText("HPF");
        ui.dlyParam6->setRange(0, 52);
        ui.dlyParam6->setValue(0);
        ui.dlyParam6->setValueTextType(DialKnob::EqFrequency);

        ui.dlyLabel7->setText("LPF");
        ui.dlyParam7->show();
        ui.dlyParam7->setRange(34, 60);
        ui.dlyParam7->setValue(54);
        ui.dlyParam7->setValueTextType(DialKnob::EqFrequency);

        break;

    case AN1x::LR:

        ui.dlyLabel1->setText("Left");
        ui.dlyParam1->setRange(0, 6599);
        ui.dlyParam1->setValue(3914);
        ui.dlyParam1->setOffset(1);
        ui.dlyParam1->setValueTextType(DialKnob::Milliseconds);

        ui.dlyLabel2->setText("Right");
        ui.dlyParam2->setRange(0, 6599);
        ui.dlyParam2->setValue(4121);
        ui.dlyParam2->setOffset(1);
        ui.dlyParam2->setValueTextType(DialKnob::Milliseconds);

        ui.dlyLabel3->setText("FB Delay 1");
        ui.dlyParam3->setRange(0, 6599);
        ui.dlyParam3->setValue(3914);
        ui.dlyParam3->setOffset(1);
        ui.dlyParam3->setValueTextType(DialKnob::Milliseconds);

        ui.dlyLabel4->setText("FB Delay 2");
        ui.dlyParam4->setRange(0, 6599);
        ui.dlyParam4->setValue(4121);
        ui.dlyParam4->setOffset(1);
        ui.dlyParam4->setValueTextType(DialKnob::Milliseconds);

        ui.dlyLabel5->setText("FB Level");
        ui.dlyParam5->setRange(0, 198);
        ui.dlyParam5->setValue(129);
        ui.dlyParam5->setSuffix("%");
        ui.dlyParam5->setOffset(-99);

        ui.dlyLabel6->setText("HPF");
        ui.dlyParam6->setRange(0, 52);
        ui.dlyParam6->setValue(0);
        ui.dlyParam6->setValueTextType(DialKnob::EqFrequency);

        ui.dlyLabel7->setText("LPF");
        ui.dlyParam7->show();
        ui.dlyParam7->setRange(34, 60);
        ui.dlyParam7->setValue(54);
        ui.dlyParam7->setValueTextType(DialKnob::EqFrequency);

        break;

    case AN1x::Echo:
        ui.dlyLabel1->setText("Left");
        ui.dlyParam1->setRange(0, 3299);
        ui.dlyParam1->setValue(3239);
        ui.dlyParam1->setOffset(1);
        ui.dlyParam1->setValueTextType(DialKnob::Milliseconds);

        ui.dlyLabel2->setText("Left FB");
        ui.dlyParam2->setRange(0, 198);
        ui.dlyParam2->setValue(134);
        ui.dlyParam2->setOffset(-99);
        ui.dlyParam2->setSuffix("%");

        ui.dlyLabel3->setText("Right");
        ui.dlyParam3->setRange(0, 3299);
        ui.dlyParam3->setValue(3299);
        ui.dlyParam3->setOffset(1);
        ui.dlyParam3->setValueTextType(DialKnob::Milliseconds);

        ui.dlyLabel4->setText("Rright FB");
        ui.dlyParam4->setRange(0, 198);
        ui.dlyParam4->setValue(134);
        ui.dlyParam4->setOffset(-99);
        ui.dlyParam4->setSuffix("%");

        ui.dlyLabel5->setText("HPF");
        ui.dlyParam5->setRange(0, 52);
        ui.dlyParam5->setValue(0);
        ui.dlyParam5->setValueTextType(DialKnob::EqFrequency);

        ui.dlyLabel6->setText("LPF");
        ui.dlyParam6->setRange(34, 60);
        ui.dlyParam6->setValue(54);
        ui.dlyParam6->setValueTextType(DialKnob::EqFrequency);

        ui.dlyLabel7->setText("");
        ui.dlyParam7->hide();

        break;

    case AN1x::Cross:
        ui.dlyLabel1->setText("Left->Right");
        ui.dlyParam1->setRange(0, 3299);
        ui.dlyParam1->setValue(3299);
        ui.dlyParam1->setOffset(1);
        ui.dlyParam1->setValueTextType(DialKnob::Milliseconds);

        ui.dlyLabel2->setText("L->R FB");
        ui.dlyParam2->setRange(0, 198);
        ui.dlyParam2->setValue(142);
        ui.dlyParam2->setOffset(-99);
        ui.dlyParam2->setSuffix("%");

        ui.dlyLabel3->setText("Right->Left");
        ui.dlyParam3->setRange(0, 3299);
        ui.dlyParam3->setValue(3299);
        ui.dlyParam3->setOffset(1);
        ui.dlyParam3->setValueTextType(DialKnob::Milliseconds);

        ui.dlyLabel4->setText("R->L FB");
        ui.dlyParam4->setRange(0, 198);
        ui.dlyParam4->setValue(142);
        ui.dlyParam4->setOffset(-99);
        ui.dlyParam4->setSuffix("%");

        ui.dlyLabel5->setText("Input");
        ui.dlyParam5->setRange(0, 2);
        ui.dlyParam5->setValue(0);
        ui.dlyParam5->setNotchesVisible(true);
        ui.dlyParam5->setNotchTarget(1);
        ui.dlyParam5->setValueTextType(DialKnob::DelayInput);

        ui.dlyLabel6->setText("HPF");
        ui.dlyParam6->setRange(0, 52);
        ui.dlyParam6->setValue(0);
        ui.dlyParam6->setValueTextType(DialKnob::EqFrequency);

        ui.dlyLabel7->setText("LPF");
        ui.dlyParam7->show();
        ui.dlyParam7->setRange(34, 60);
        ui.dlyParam7->setValue(54);
        ui.dlyParam7->setValueTextType(DialKnob::EqFrequency);

        break;

    case AN1x::TempoDelay:
        ui.dlyLabel1->setText("Subdivide");
        ui.dlyParam1->setRange(0, 10);
        ui.dlyParam1->setValue(2);
        ui.dlyParam1->setNotchesVisible(true);
        ui.dlyParam1->setNotchTarget(1);
        ui.dlyParam1->setValueTextType(DialKnob::TempoDelay);

        ui.dlyLabel2->setText("L Diffusion");
        ui.dlyParam2->setRange(44, 84);
        ui.dlyParam2->setOffset(-64);
        ui.dlyParam2->setValue(63);
        ui.dlyParam2->showPlusOnPositives(true);
        ui.dlyParam2->setSuffix("%");

        ui.dlyLabel3->setText("R Diffusion");
        ui.dlyParam3->setRange(44, 84);
        ui.dlyParam3->setOffset(-64);
        ui.dlyParam3->setValue(63);
        ui.dlyParam3->showPlusOnPositives(true);
        ui.dlyParam3->setSuffix("%");

        ui.dlyLabel4->setText("Feedback");
        ui.dlyParam4->setRange(0, 198);
        ui.dlyParam4->setValue(132);
        ui.dlyParam4->setOffset(-99);
        ui.dlyParam4->setSuffix("%");

        ui.dlyLabel5->setText("HPF");
        ui.dlyParam5->setRange(0, 52);
        ui.dlyParam5->setValue(0);
        ui.dlyParam5->setValueTextType(DialKnob::EqFrequency);

        ui.dlyLabel6->setText("LPF");
        ui.dlyParam6->setRange(34, 60);
        ui.dlyParam6->setValue(54);
        ui.dlyParam6->setValueTextType(DialKnob::EqFrequency);

        ui.dlyLabel7->setText("");
        ui.dlyParam7->hide();

        break;
    };



    ui.dlyParam1->setCurrentValueAsDefault();
    ui.dlyParam2->setCurrentValueAsDefault();
    ui.dlyParam3->setCurrentValueAsDefault();
    ui.dlyParam4->setCurrentValueAsDefault();
    ui.dlyParam5->setCurrentValueAsDefault();
    ui.dlyParam6->setCurrentValueAsDefault();
    ui.dlyParam7->setCurrentValueAsDefault();
}

void FxEq::setReverbLayout()
{
    ui.revParam1->setValueTextType(DialKnob::ReverbTime);
    ui.revParam2->setValueTextType(DialKnob::ReverbDamp);
    ui.revParam2->setOffset(1);
    ui.revParam4->setValueTextType(DialKnob::Milliseconds);
    ui.revParam4->setOffset(1);
    ui.revParam5->setOffset(-64);
    ui.revParam5->showPlusOnPositives(true);
    ui.revParam6->setValueTextType(DialKnob::EqFrequency);
    ui.revParam7->setValueTextType(DialKnob::EqFrequency);

}

void FxEq::setEqLayout()
{
    ui.lowFq->setValueTextType(DialKnob::EqFrequency);
    ui.midFq->setValueTextType(DialKnob::EqFrequency);
    ui.highFq->setValueTextType(DialKnob::EqFrequency);
    ui.midRes->setValueTextType(DialKnob::Resonance);
    ui.lowGain->setOffset(-64);
    ui.lowGain->setSuffix("dB");
    ui.lowGain->showPlusOnPositives(true);
    ui.midGain->setOffset(-64);
    ui.midGain->setSuffix("dB");
    ui.midGain->showPlusOnPositives(true);
    ui.highGain->setOffset(-64);
    ui.highGain->setSuffix("dB");
    ui.highGain->showPlusOnPositives(true);

}

void FxEq::setBypass()
{
    bool all = ui.allBypass->isChecked();
    
    if (all) {
        QSignalBlocker d(ui.dlyBypass);
        QSignalBlocker r(ui.revBypass);
        ui.dlyBypass->setChecked(true);
        ui.revBypass->setChecked(true);
        MidiMaster::setParam(ParamType::System, AN1x::EffectBypass, 4);
    }

    bool dly = ui.dlyBypass->isChecked();
    bool rev = ui.revBypass->isChecked();

    bool value[4] = { (!dly && !rev),(!dly && rev),(dly && !rev),(dly && rev) };

    for (int i = 0; i < 4; i++) {
        if (value[i]) {
            MidiMaster::setParam(ParamType::System, AN1x::EffectBypass, i);
        }
    }

}


FxEq::~FxEq()
{}
