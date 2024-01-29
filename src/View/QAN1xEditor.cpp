#include "QAN1xEditor.h"

#include <QUrl>
#include <QKeyEvent>
#include <QDesktopServices>

#include "Database/Database.h"
#include "GlobalWidgets.h"
#include "Model/MidiMaster.h"
#include "Model/An1xPatch.h"

#include "FreeFunctions.h"

QAN1xEditor::QAN1xEditor(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    ui.an1xPng->setStyleSheet("background-image: url(:/resources/an1x.png)");

    installEventFilter(this);
    initializeInitMenu();

    GlobalWidgets::statusBar = statusBar();

    ui.pitchBend->setCurrentValueAsDefault();
    ui.modWheel->setCurrentValueAsDefault();

    for (int i = 0; i < ui.voiceType->count(); i++) {
        ui.voiceType->setItemIcon(i, FreeFn::getTypeIcon(i));
    }

    ui.pianoView->setOctave(ui.pcKbdOctave->value());

    connect(ui.donateButton, &QPushButton::clicked, [&] { QDesktopServices::openUrl(QUrl("https://www.paypal.com/donate/?hosted_button_id=NW5FHTBR8FG56", QUrl::TolerantMode)); });
    
    connect(ui.modWheel, &QSlider::valueChanged, [this](int value) { MidiMaster::modWheelChange(value); });
    connect(ui.pitchBend, &QSlider::valueChanged, [this](int value) { MidiMaster::pitchChange(value); });
    connect(ui.velocityKbdSpin, &QSpinBox::valueChanged, [=](int value) { ui.pianoView->setVelocity(value); });

    connect(ui.pcKbdOctave, &QSpinBox::valueChanged, [this](int value) { 
            ui.pitchBend->setValue(64);
            ui.modWheel->setValue(0);
            ui.pianoView->setOctave(value);
            MidiMaster::setKbdOctave(value); 
    });

    connect(ui.requestSystem, &QPushButton::clicked, [this] { MidiMaster::requestSystem(); });
    connect(ui.sendSystem, &QPushButton::clicked, [this] { MidiMaster::sendSystem(); });
    connect(ui.restoreSystem, &QPushButton::clicked, [this] { MidiMaster::restoreSystem(); });
    connect(ui.panikButton, &QPushButton::clicked, [this] { 

        ui.pitchBend->setValue(64);
        ui.modWheel->setValue(0);
        MidiMaster::stopAllSounds(); 
    });

    connect(ui.enablePcKbd, &QCheckBox::stateChanged, [this](bool checked) { 
        ui.pcKbdOctave->setHidden(!checked);
        ui.octaveDescrLabel->setHidden(!checked);
        ui.octaveLabel->setHidden(!checked);
        ui.pianoView->setOctave(checked ? ui.pcKbdOctave->value() : -3);
    });

    ui.enablePcKbd->stateChanged(false);

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
    connect(ui.outCombo, &QComboBox::currentIndexChanged, [=](int index) { MidiMaster::connectMidiOut(index - 1); /*MidiMaster::syncBulk();*/ });

    //LAYER
    connect(ui.single, &QRadioButton::clicked, [=] { MidiMaster::parameterChanged(ParamType::Common, AN1x::LayerMode, !ui.unison->isChecked() ? AN1x::Single : AN1x::Unison); });
    connect(ui.dual, &QRadioButton::clicked, [=] { MidiMaster::parameterChanged(ParamType::Common, AN1x::LayerMode, !ui.unison->isChecked() ? AN1x::Dual : AN1x::DualUnison); });
    connect(ui.split, &QRadioButton::clicked, [=] { MidiMaster::parameterChanged(ParamType::Common, AN1x::LayerMode, !ui.unison->isChecked() ? AN1x::Split : AN1x::SplitUnison); });
    connect(ui.unison, &QGroupBox::clicked, [=](bool checked) {

        if (ui.single->isChecked())
        {
            MidiMaster::parameterChanged(ParamType::Common, AN1x::LayerMode, !checked ? AN1x::Single : AN1x::Unison);
        }
        else if (ui.dual->isChecked())
        {
            MidiMaster::parameterChanged(ParamType::Common, AN1x::LayerMode, !checked ? AN1x::Dual : AN1x::Unison);
        }
        else if (ui.split->isChecked())
        {
            MidiMaster::parameterChanged(ParamType::Common, AN1x::LayerMode, !checked ? AN1x::Split : AN1x::SplitUnison);
        }
        }
    );

    connect(ui.scene1radio, &QRadioButton::clicked, [=] { MidiMaster::parameterChanged(ParamType::Common, AN1x::SceneSelect, 0); });
    connect(ui.scene2radio, &QRadioButton::clicked, [=] { MidiMaster::parameterChanged(ParamType::Common, AN1x::SceneSelect, 1); });
    connect(ui.bothSceneRadio, &QRadioButton::clicked, [=] { MidiMaster::parameterChanged(ParamType::Common, AN1x::SceneSelect, 2); });

    connect(ui.transpose, &QSpinBox::valueChanged, [=](int value) { ui.transpose->setPrefix(value > 0 ? "+" : ""); });

    connect(ui.pitchBend, &QSlider::sliderReleased, [&] { ui.pitchBend->setValue(64); });

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
        ui_controls[i]->setParam(ParamType::Common, (AN1x::CommonParam)i);
    }

    ui.arpSeqCheck->setCurrentValueAsDefault();
    ui.arpSeqCheck->setParam(ParamType::Common, AN1x::ArpSeqOnOff);

    system_controls[0] = ui.masterTune;
    system_controls[2] = ui.transpose;
    system_controls[3] = ui.velocityCurve;
    system_controls[4] = ui.fixedVelocity;
    system_controls[11] = ui.midiLocal;

    for (int i = 0; i < system_controls.size(); i++)
    {
        if (system_controls[i] == nullptr) continue;

        system_controls[i]->setCurrentValueAsDefault();
        system_controls[i]->setParam(ParamType::System, (AN1x::SystemParam)i);
    }


    MidiMaster::setView(this);

    MidiMaster::refreshConnection();


}

void QAN1xEditor::setPatch(const AN1xPatch& patch)
{

    for (int i = 0; i < AN1x::FreeEgData; i++) {
        if (AN1x::isNull(ParamType::Common, i)) continue;
        setParameter(ParamType::Common, i, patch.getParameter(ParamType::Common, i));
    }

    for (int i = 0; i < AN1x::SceneSize; i++) {
        if (AN1x::isNull(ParamType::Scene1, i)) continue;
        setParameter(ParamType::Scene1, i, patch.getParameter(ParamType::Scene1, i));
    }

    for (int i = 0; i < AN1x::SceneSize; i++){
        if (AN1x::isNull(ParamType::Scene2, i)) continue;
        setParameter(ParamType::Scene2, i, patch.getParameter(ParamType::Scene2, i));
    }

    for (int i = 0; i < AN1x::SeqencerSize; i++) {
        if (AN1x::isNull(ParamType::StepSq, i)) continue;
        setParameter(ParamType::StepSq, i, patch.getParameter(ParamType::StepSq, i));
    }

    ui.FreeEG->setTrackData(patch.getFreeEGData());

    GlobalWidgets::statusBar->clearMessage();

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

void QAN1xEditor::setParameter(ParamType type, unsigned char param, int value)
{
    switch (type)
    {
        case ParamType::System: setSystemParameter((AN1x::SystemParam)param, value); break;
        case ParamType::Common: setCommonParameter((AN1x::CommonParam)param, value); break;
        case ParamType::Scene1: setSceneParameter((AN1x::SceneParam)param, value, false); break;
        case ParamType::Scene2: setSceneParameter((AN1x::SceneParam)param, value, true); break;
        case ParamType::StepSq: setSequenceParameter((AN1x::SeqParam)param, value); break;
    }
}

void QAN1xEditor::setModWheel(int value)
{
    ui.modWheel->setValue(value);
}

void QAN1xEditor::setTrackData(const std::vector<int>& trackData)
{
    ui.FreeEG->setTrackData(trackData);
}


Browser* QAN1xEditor::browser()
{
    return ui.browser;
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
    if (p >= AN1x::SceneSize) return;

    auto& sceneView = isScene2 ? *ui.scene2tab : *ui.scene1tab;

    sceneView.setSceneParameters(p, value);
    
    auto& ctrlMatrix = isScene2 ? *ui.ctrlMatrixScene1 : *ui.ctrlMatrixScene2;

    ctrlMatrix.setSceneParameters(p, value);

    ui.fxeqTab->setSceneParameter(p, value, isScene2);
}

void QAN1xEditor::setCommonParameter(AN1x::CommonParam p, int value)
{
    if (p >= AN1x::FreeEgData) return;

    //name edit
    if (p <= AN1x::Name10) {
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
    if (p >= AN1x::SeqencerSize) return;
    ui.seqTab->setSequenceParameter(p, value);
}

void QAN1xEditor::initializeInitMenu()
{

    const char* strArr[] = {
        "Init Normal", "Init Bass", "Init Brass",  "Init String", "Init E.Piano", "Init Organ", "Init Sync", "Init PWM"
    };

    int iconArr[] = { 0, 5, 8, 6, 1, 3, 11, 12 };

    for (int i = 0; i < 8; i++) {

        auto action = new QAction(strArr[i]);

        action->setIcon(FreeFn::getTypeIcon(iconArr[i]));

        connect(action, &QAction::triggered, this, [=]{
            MidiMaster::newPatch((AN1x::InitType)i);
         });

        ui.initButton->addAction(action);
    }
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

    static const int keyboardKeys[] = {
            65, //Qt::Key_A,
            87, //Qt::Key_W,
            83, //Qt::Key_S,
            69, //Qt::Key_E,
            68, //Qt::Key_D,
            70, //Qt::Key_F,
            84, //Qt::Key_T,
            71, //Qt::Key_G,
            89, //Qt::Key_Y,
            72, //Qt::Key_H,
            85, //Qt::Key_U,
            74, //Qt::Key_J,
            75, //Qt::Key_K,
            79, //Qt::Key_O,
            76, //Qt::Key_L,
            80, //Qt::Key_P,
            186,//Qt::Key_Semicolon,
            222,//Qt::Key_Apostrophe,
            221,//Qt::Key_BracketRight,
            220,//Qt::Key_Backslash
            88, //Qt::Key_X
            90  //Qt::Key_Z
    };

    if (e->type() == QEvent::KeyPress)
    {
        auto keyEvent = static_cast<QKeyEvent*>(e);
/*
        bool notAKey = false;

        for (auto key : keyboardKeys) {

            if (key == keyEvent->nativeVirtualKey()) {
                notAKey = true;
                break;
            }
        }

        if (!notAKey) {
            releaseKeyboard();
            return false;
        }
        */
        if (keyEvent->modifiers()) goto here;

        if (keyEvent->isAutoRepeat()) goto here;

        grabKeyboard();

        if (keyEvent->key() == Qt::Key_X) {
            ui.pcKbdOctave->setValue(ui.pcKbdOctave->value() + 1);
        }
        else if (keyEvent->key() == Qt::Key_Z) {
            ui.pcKbdOctave->setValue(ui.pcKbdOctave->value() - 1);
        }
        else {
            MidiMaster::pcKeyPress(keyEvent->key(), true, ui.velocityKbdSpin->value());
        }

        return false;
    }

    if (e->type() == QEvent::KeyRelease)
    {
        auto keyEvent = static_cast<QKeyEvent*>(e);

        if (keyEvent->modifiers()) goto here;

        if (keyEvent->isAutoRepeat()) goto here;

        MidiMaster::pcKeyPress(keyEvent->key(), false, ui.velocityKbdSpin->value());

        return false;
    }

here:

    return QWidget::eventFilter(obj, e);
 
}


QAN1xEditor::~QAN1xEditor()
{
    MidiMaster::stopAllSounds();
}
