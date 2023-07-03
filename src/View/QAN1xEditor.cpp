#include "QAN1xEditor.h"
#include "qmidimessage.h"
#include "GlobalWidgets.h"
#include "Model/MidiMaster.h"

#include <QKeyEvent>


QAN1xEditor::QAN1xEditor(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.an1xPng->setStyleSheet("background-image: url(:QMidiAn1x/an1x.png)");

    installEventFilter(this);

    GlobalWidgets::statusBar = statusBar();

    MidiMaster::setView(this);

    ui.pitchBend->setCurrentValueAsDefault();
    ui.modWheel->setCurrentValueAsDefault();

    connect(ui.modWheel, &QSlider::valueChanged, [this](int value) { MidiMaster::modWheelChange(value); });
    connect(ui.pitchBend, &QSlider::valueChanged, [this](int value) { MidiMaster::pitchChange(value); });
    connect(ui.velocityKbdSpin, &QSpinBox::valueChanged, [=](int value) { ui.pianoView->setVelocity(value); });
    connect(ui.pcKbdOctave, &QSpinBox::valueChanged, [this](int value) { MidiMaster::setKbdOctave(value); });
    connect(ui.requestVoice, &QPushButton::clicked, [this] { MidiMaster::syncBulk(); });

    connect(ui.requestVoiceNum, &QPushButton::clicked, [this] { MidiMaster::goToVoice(ui.voiceSpin->value()-1); });

    ui.octaveDescrLabel->hide();
    connect(ui.enablePcKbd, &QCheckBox::stateChanged, [this](bool checked) { ui.octaveDescrLabel->setHidden(!checked); });

    ui.scene1tab->setAsScene(false);
    ui.scene2tab->setAsScene(true);
    ui.ctrlMatrixScene1->setAsScene(false);
    ui.ctrlMatrixScene2->setAsScene(true);

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
    connect(ui.refresh, &QPushButton::clicked, [=] { MidiMaster::refreshConnection(); });
    connect(ui.inCombo, &QComboBox::currentIndexChanged, [=](int index) { MidiMaster::connectMidiIn(index - 1); });
    connect(ui.outCombo, &QComboBox::currentIndexChanged, [=](int index) { MidiMaster::connectMidiOut(index - 1); MidiMaster::syncBulk(); });

    //LAYER
    connect(ui.single, &QRadioButton::clicked, [=] { MidiMaster::setParam(AN1x::ParamType::Common, AN1x::LayerMode, !ui.unison->isChecked() ? AN1x::Single : AN1x::Unison); });
    connect(ui.dual, &QRadioButton::clicked, [=] { MidiMaster::setParam(AN1x::ParamType::Common, AN1x::LayerMode, !ui.unison->isChecked() ? AN1x::Dual : AN1x::DualUnison); });
    connect(ui.split, &QRadioButton::clicked, [=] { MidiMaster::setParam(AN1x::ParamType::Common, AN1x::LayerMode, !ui.unison->isChecked() ? AN1x::Split : AN1x::SplitUnison); });
    connect(ui.unison, &QGroupBox::clicked, [=](bool checked) {

        if (ui.single->isChecked())
        {
            MidiMaster::setParam(AN1x::ParamType::Common, AN1x::LayerMode, !checked ? AN1x::Single : AN1x::Unison);
        }
        else if (ui.dual->isChecked())
        {
            MidiMaster::setParam(AN1x::ParamType::Common, AN1x::LayerMode, !checked ? AN1x::Dual : AN1x::Unison);
        }
        else if (ui.split->isChecked())
        {
            MidiMaster::setParam(AN1x::ParamType::Common, AN1x::LayerMode, !checked ? AN1x::Split : AN1x::SplitUnison);
        }
        }
    );

    connect(ui.scene1radio, &QRadioButton::clicked, [=] { MidiMaster::setParam(AN1x::ParamType::Common, AN1x::SceneSelect, 0); });
    connect(ui.scene2radio, &QRadioButton::clicked, [=] { MidiMaster::setParam(AN1x::ParamType::Common, AN1x::SceneSelect, 1); });
    connect(ui.bothSceneRadio, &QRadioButton::clicked, [=] { MidiMaster::setParam(AN1x::ParamType::Common, AN1x::SceneSelect, 2); });

    connect(ui.transpose, &QSpinBox::valueChanged, [=](int value) { ui.transpose->setPrefix(value > 0 ? "+" : ""); });


    ui.fixedVelocity->setSpecialValueText("Off");
    ui.splitPoint->setAsNoteCombo();
    ui.tempoBPM->setSpecialValueText("MIDI");
    ui.masterTune->setValueTextType(DialKnob::MasterTune);

    connect(ui.fixedVelocity, &QSpinBox::valueChanged, 
        [&](int value) { 
            ui.velocityCurve->setDisabled(value); 
            ui.velCurveLabel->setDisabled(value);
        });



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
        ui.tempoBPM,
        nullptr,
        ui.splitPoint,
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
        ui_controls[i]->setParam(AN1x::ParamType::Common, (AN1x::CommonParam)i);
    }

    ui.arpSeqCheck->setCurrentValueAsDefault();
    ui.arpSeqCheck->setParam(AN1x::ParamType::Common, AN1x::ArpSeqOnOff);

    system_controls[0] = ui.masterTune;
    system_controls[2] = ui.transpose;
    system_controls[3] = ui.velocityCurve;
    system_controls[4] = ui.fixedVelocity;
    system_controls[11] = ui.midiLocal;

    for (int i = 0; i < system_controls.size(); i++)
    {
        if (system_controls[i] == nullptr) continue;

        system_controls[i]->setCurrentValueAsDefault();
        system_controls[i]->setParam(AN1x::ParamType::System, (AN1x::SystemParam)i);
    }
 
    MidiMaster::refreshConnection();


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

void QAN1xEditor::setParameter(AN1x::ParamType type, unsigned char param, int value)
{
    switch (type)
    {
        case AN1x::ParamType::System: setSystemParameter((AN1x::SystemParam)param, value); break;
        case AN1x::ParamType::Common: setCommonParameter((AN1x::CommonParam)param, value); break;
        case AN1x::ParamType::Scene1: setSceneParameter((AN1x::SceneParam)param, value, false); break;
        case AN1x::ParamType::Scene2: setSceneParameter((AN1x::SceneParam)param, value, true); break;
        case AN1x::ParamType::StepSq: setSequenceParameter((AN1x::SeqParam)param, value); break;
    }
}

void QAN1xEditor::setModWheel(int value)
{
    ui.modWheel->setValue(value);
}

void QAN1xEditor::setSystemParameter(AN1x::SystemParam p, int value)
{
    if (p >= AN1x::SystemMaxSize) return;

    if (p == AN1x::SystemParam::VelocityCurve)
    {
        QSignalBlocker b(ui.fixedVelocity);
        ui.fixedVelocity->setValue(0);
        ui.velocityCurve->setDisabled(false);
        ui.velCurveLabel->setDisabled(false);
    }

    ui.fxeqTab->setSystemParameter(p, value);

    if (system_controls[p] == nullptr) return;

    system_controls[p]->setValue(value);
 
}

void QAN1xEditor::setSceneParameter(AN1x::SceneParam p, int value, bool isScene2)
{
    if (p >= AN1x::SceneParametersMaxSize) return;

    auto& sceneView = isScene2 ? *ui.scene2tab : *ui.scene1tab;

    sceneView.setSceneParameters(p, value);
    
    auto& ctrlMatrix = isScene2 ? *ui.ctrlMatrixScene1 : *ui.ctrlMatrixScene2;

    ctrlMatrix.setSceneParameters(p, value);

    ui.fxeqTab->setSceneParameter(p, value, isScene2);
}

void QAN1xEditor::setCommonParameter(AN1x::CommonParam p, int value)
{
    if (p >= AN1x::CommonMaxSize) return;

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


    if (p == AN1x::ArpSeqOnOff) {
        ui.arpSeqCheck->setValue(value);
        return;
    }

    if (p == AN1x::SceneSelect) {
       
        QRadioButton* group[3]{ ui.scene1radio, ui.scene2radio, ui.bothSceneRadio };
        group[value]->setChecked(true);
        return;
    }

    if (p < AN1x::VariFXType && ui_controls[p] != nullptr) {
        ui_controls[p]->setValue(value);
    }

    ui.fxeqTab->setCommonParameter(p, value);
    ui.seqTab->setCommonParameter(p, value);
    ui.FreeEG->setCommonParameter(p, value);
}

void QAN1xEditor::setSequenceParameter(AN1x::SeqParam p, int value)
{
    if (p >= AN1x::StepSequencerMaxSize) return;
    ui.seqTab->setSequenceParameter(p, value);
}

unsigned char QAN1xEditor::layerMode()
{
    bool notUnison = !ui.unison->isChecked();

    if (ui.single->isChecked()) return notUnison ? 0 : 1;
    if (ui.dual->isChecked()) return notUnison ? 2 : 3;
    if (ui.split->isChecked()) return notUnison ? 4 : 5;

    return 0;
}

PianoView* QAN1xEditor::pianoRoll()
{
    return ui.pianoView;
}



bool QAN1xEditor::eventFilter(QObject* obj, QEvent* e)
{
    if (!ui.enablePcKbd->isChecked()) {
        releaseKeyboard();
        return false;
    }

    if (e->type() == QEvent::KeyPress)
    {
        auto keyEvent = static_cast<QKeyEvent*>(e);

        if (keyEvent->modifiers()) goto here;

        if (keyEvent->isAutoRepeat()) goto here;

        grabKeyboard();

        if (keyEvent->nativeVirtualKey() == Qt::Key_X) {
            ui.pcKbdOctave->setValue(ui.pcKbdOctave->value() + 1);
        }
        else if (keyEvent->nativeVirtualKey() == Qt::Key_Z) {
            ui.pcKbdOctave->setValue(ui.pcKbdOctave->value() - 1);
        }
        else {
            MidiMaster::pcKeyPress(keyEvent->nativeVirtualKey(), true, ui.velocityKbdSpin->value());
        }

        return false;
    }

    if (e->type() == QEvent::KeyRelease)
    {
        auto keyEvent = static_cast<QKeyEvent*>(e);

        if (keyEvent->modifiers()) goto here;

        if (keyEvent->isAutoRepeat()) goto here;

        MidiMaster::pcKeyPress(keyEvent->nativeVirtualKey(), false, ui.velocityKbdSpin->value());

        return false;
    }

here:

    return QWidget::eventFilter(obj, e);
 
}

std::vector<int> QAN1xEditor::getCommon()
{
    
    int scene = 0;
    if (ui.scene2radio->isChecked()) scene = 1;
    if (ui.bothSceneRadio->isChecked()) scene = 2;

    int arpSeqType = ui.seqTab->ui.radioArp->isChecked() ? 0 : 1;

    int seqSceneSwitch = 0;
    if (ui.seqTab->ui.radioScene2) seqSceneSwitch = 1;
    if (ui.seqTab->ui.radioBothScenes) seqSceneSwitch = 2;

    std::vector<int> result{
        (int)ui.voiceNameEdit->getChar(0),
        (int)ui.voiceNameEdit->getChar(1),
        (int)ui.voiceNameEdit->getChar(2),
        (int)ui.voiceNameEdit->getChar(3),
        (int)ui.voiceNameEdit->getChar(4),
        (int)ui.voiceNameEdit->getChar(5),
        (int)ui.voiceNameEdit->getChar(6),
        (int)ui.voiceNameEdit->getChar(7),
        (int)ui.voiceNameEdit->getChar(8),
        (int)ui.voiceNameEdit->getChar(9),
        ui.voiceType->getValue(),
        scene,
        layerMode(),
        ui.layerPan->getValue(),
        ui.separation->getValue(),
        ui.detune->getValue(),
        ui.tempoBPM->getValue(),
        0x00,
        ui.splitPoint->getValue(),
        ui.portamentoCheck->getValue(),
        ui.sourceMtrxCom_1->getValue(),
        ui.paramMtrxCom_1->getValue(),
        ui.depthMtrxCom_1->getValue(),
        ui.sourceMtrxCom_2->getValue(),
        ui.paramMtrxCom_2->getValue(),
        ui.depthMtrxCom_2->getValue(),
        ui.fxeqTab->ui.variFxType->getValue(),
        0x00,
        ui.fxeqTab->ui.fxParam1->getValue(),
        0x00,
        ui.fxeqTab->ui.fxParam2->getValue(),
        0x00,
        ui.fxeqTab->ui.fxParam3->getValue(),
        0x00,
        ui.fxeqTab->ui.fxParam4->getValue(),
        0x00,
        ui.fxeqTab->ui.fxParam5->getValue(),
        0x00,
        ui.fxeqTab->ui.fxParam6->getValue(),
        0x00,
        ui.fxeqTab->ui.lowFq->getValue(),
        ui.fxeqTab->ui.lowGain->getValue(),
        ui.fxeqTab->ui.midFq->getValue(),
        ui.fxeqTab->ui.midGain->getValue(),
        ui.fxeqTab->ui.highFq->getValue(),
        ui.fxeqTab->ui.highGain->getValue(),
        ui.fxeqTab->ui.DlyRevConnection->getValue(),
        ui.fxeqTab->ui.delayType->getValue(),
        ui.fxeqTab->ui.dlyReturn->getValue(),
        ui.fxeqTab->ui.dlyParam1->getValue(),
        0x00,
        ui.fxeqTab->ui.dlyParam1->getValue(),
        0x00,
        ui.fxeqTab->ui.dlyParam2->getValue(),
        0x00,
        ui.fxeqTab->ui.dlyParam3->getValue(),
        0x00,
        ui.fxeqTab->ui.dlyParam4->getValue(),
        0x00,
        ui.fxeqTab->ui.dlyParam5->getValue(),
        0x00,
        ui.fxeqTab->ui.dlyParam6->getValue(),
        0x00,
        ui.fxeqTab->ui.dlyParam7->getValue(),
        0x00,
        ui.fxeqTab->ui.reverbType->getValue(),
        ui.fxeqTab->ui.revReturn->getValue(),
        ui.fxeqTab->ui.revParam1->getValue(),
        0x00,
        ui.fxeqTab->ui.revParam2->getValue(),
        0x00,
        ui.fxeqTab->ui.revParam3->getValue(),
        0x00,
        ui.fxeqTab->ui.revParam4->getValue(),
        0x00,
        ui.fxeqTab->ui.revParam5->getValue(),
        0x00,
        ui.fxeqTab->ui.revParam6->getValue(),
        0x00,
        ui.fxeqTab->ui.revParam7->getValue(),
        0x00,
        ui.arpSeqCheck->getValue(),
        arpSeqType,
        arpSeqType ? ui.seqTab->ui.seqPatternNo->getValue() : ui.seqTab->ui.arpType->getValue() ,
        ui.seqTab->ui.kbdMode->getValue(),
        seqSceneSwitch,
        ui.seqTab->ui.arpSubdivide->getValue(),
        ui.seqTab->ui.swing->getValue(),
        ui.seqTab->ui.velocity->getValue(),
        0x00,
        ui.seqTab->ui.gate->getValue(),
        0x00,
        ui.FreeEG->ui.triggerCombo->getValue(),
        ui.FreeEG->ui.loopCombo->getValue(),
        ui.FreeEG->ui.lengthCombo->getValue(),
        ui.FreeEG->ui.keyTrack->getValue(),
        ui.FreeEG->ui.trackParam_1->getValue(),
        ui.FreeEG->ui.trackSceneSw_1->getValue(),
        ui.FreeEG->ui.trackParam_2->getValue(),
        ui.FreeEG->ui.trackSceneSw_2->getValue(),
        ui.FreeEG->ui.trackParam_3->getValue(),
        ui.FreeEG->ui.trackSceneSw_3->getValue(),
        ui.FreeEG->ui.trackParam_4->getValue(),
        ui.FreeEG->ui.trackSceneSw_4->getValue()
    };

    auto tracks = ui.FreeEG->getTrackData();

    result.insert(result.end(), tracks.begin(), tracks.end());

    return result;
}



QAN1xEditor::~QAN1xEditor()
{}
