#include "QAN1xEditor.h"

#include <QUrl>
#include <QKeyEvent>
#include <QDesktopServices>
#include <QtVersion>
#include "GlobalWidgets.h"
#include "Model/MidiMaster.h"
#include "Model/An1xPatch.h"
#include "View/SettingsDialog.h"

#include "FreeFunctions.h"
#include "Model/PatchDatabase.h"

QAN1xEditor::QAN1xEditor(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    GlobalWidgets::statusBar = statusBar();

    installEventFilter(this);
    initializeInitMenu();

    ui.donateButton->setIcon(QPixmap(":/icon/resources/icon_fav.png"));

    ui.pitchBend->setCurrentValueAsDefault();
    ui.modWheel->setCurrentValueAsDefault();
    ui.saveButton->setDisabled(true);

    for (int i = 0; i < ui.voiceType->count(); i++) {
        ui.voiceType->setItemIcon(i, FreeFn::getTypeIcon(i));
    }

    ui.pianoView->setOctave(ui.pcKbdOctave->value());

    connect(ui.donateButton, &QPushButton::clicked, this, [&] { QDesktopServices::openUrl(QUrl("https://www.paypal.com/donate/?hosted_button_id=NW5FHTBR8FG56", QUrl::TolerantMode)); });

    connect(ui.modWheel, &QSlider::valueChanged, this, [&](int value) { MidiMaster::modWheelChange(value); });
    connect(ui.pitchBend, &QSlider::valueChanged, this, [&](int value) { MidiMaster::pitchChange(value); });
    connect(ui.velocityKbdSpin, &QSpinBox::valueChanged, this, [=](int value) { ui.pianoView->setVelocity(value); });

    connect(ui.pcKbdOctave, &QSpinBox::valueChanged, this, [this](int value) {
        ui.pitchBend->setValue(64);
        ui.modWheel->setValue(0);
        ui.pianoView->setOctave(value);
        MidiMaster::setKbdOctave(value);
        });

    connect(ui.dlyBypass, &QCheckBox::clicked, this, [&] { setBypass(); });
    connect(ui.revBypass, &QCheckBox::clicked, this, [&] { setBypass(); });
    connect(ui.allBypass, &QCheckBox::clicked, this, [&] { setBypass(); });

    connect(ui.requestSystem, &QPushButton::clicked, this, [&] { MidiMaster::requestSystem(); });
    connect(ui.sendSystem, &QPushButton::clicked, this, [&] { MidiMaster::sendSystem(); });
    connect(ui.restoreSystem, &QPushButton::clicked, this, [&] { MidiMaster::restoreSystem(); });
    connect(ui.panikButton, &QPushButton::clicked, this, [&] {
        MidiMaster::stopAllSounds(); 
    });

    connect(ui.saveButton, &QPushButton::clicked, this, [&] { MidiMaster::saveCurrentPatch(); });

    connect(ui.enablePcKbd, &QCheckBox::stateChanged, this, [this](bool checked) {
        ui.pcKbdOctave->setHidden(!checked);
        ui.octaveDescrLabel->setText(checked ? "(use Z and X on keyboard to change octaves)" : "");
        ui.octaveLabel->setHidden(!checked);
        ui.pianoView->setOctave(checked ? ui.pcKbdOctave->value() : -3);
    });

    emit ui.enablePcKbd->stateChanged(false);

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
    connect(ui.refresh, &QPushButton::clicked, this, [=] { MidiMaster::refreshConnection(); });
    connect(ui.inCombo, &QComboBox::currentIndexChanged, this, [=](int index) { MidiMaster::connectMidiIn(index - 1); });
    connect(ui.outCombo, &QComboBox::currentIndexChanged, this, [=](int index) { MidiMaster::connectMidiOut(index - 1); /*MidiMaster::syncBulk();*/ });

    //LAYER
    connect(ui.single, &QRadioButton::clicked, this, [=] { MidiMaster::parameterChanged(ParamType::Common, AN1x::LayerMode, !ui.unison->isChecked() ? AN1x::Single : AN1x::Unison); });
    connect(ui.dual, &QRadioButton::clicked, this, [=] { MidiMaster::parameterChanged(ParamType::Common, AN1x::LayerMode, !ui.unison->isChecked() ? AN1x::Dual : AN1x::DualUnison); });
    connect(ui.split, &QRadioButton::clicked, this, [=] { MidiMaster::parameterChanged(ParamType::Common, AN1x::LayerMode, !ui.unison->isChecked() ? AN1x::Split : AN1x::SplitUnison); });
    connect(ui.unison, &QGroupBox::clicked, this, [=](bool checked) {

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

    connect(ui.scene1radio, &QRadioButton::clicked, this, [=] { MidiMaster::parameterChanged(ParamType::Common, AN1x::SceneSelect, 0); });
    connect(ui.scene2radio, &QRadioButton::clicked, this, [=] { MidiMaster::parameterChanged(ParamType::Common, AN1x::SceneSelect, 1); });
    connect(ui.bothSceneRadio, &QRadioButton::clicked, this, [=] { MidiMaster::parameterChanged(ParamType::Common, AN1x::SceneSelect, 2); });

    connect(ui.transpose, &QSpinBox::valueChanged, this, [=](int value) { ui.transpose->setPrefix(value > 0 ? "+" : ""); });

    connect(ui.pitchBend, &QSlider::sliderReleased, this, [&] { ui.pitchBend->setValue(64); });

    ui.fixedVelocity->setSpecialValueText("Off");
    ui.splitPoint->setAsNoteCombo();
    ui.tempoBPM->setSpecialValueText("MIDI");
    ui.masterTune->setValueTextType(DialKnob::MasterTune);

    connect(ui.fixedVelocity, &QSpinBox::valueChanged, this,
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

    for (size_t i = 0; i < ui_controls.size(); i++)
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

    for (size_t i = 0; i < system_controls.size(); i++)
    {
        if (system_controls[i] == nullptr) continue;

        system_controls[i]->setCurrentValueAsDefault();
        system_controls[i]->setParam(ParamType::System, (AN1x::SystemParam)i);
    }
    
    connect(ui.settingsButton, &QPushButton::clicked, this, [&] {

        bool kbdGrab = ui.enablePcKbd->isChecked();

        if(kbdGrab){
            releaseKeyboard();
        }

        SettingsDialog d(PatchDatabase::getMidiSettings().second);

        d.exec();

        if (d.result) {
            MidiMaster::setAdvancedSettings(d.result.value());
        }

        emit ui.enablePcKbd->stateChanged(kbdGrab);
    });

#ifdef Q_OS_MAC
    ui.midiLocal->setGeometry(QRect(120, 122, 101, 21));
#endif

    MidiMaster::setView(this);
    
}

void QAN1xEditor::setPatch(const AN1xPatch& patch)
{

    for (int i = 0; i < AN1x::FreeEgData; i++) {
        if (AN1x::isNull(ParamType::Common, i)) continue;
        setParameter(ParamType::Common, i, patch.getParameter(ParamType::Common, i), true);
    }

    for (int i = 0; i < AN1x::SceneSize; i++) {
        if (AN1x::isNull(ParamType::Scene1, i)) continue;
        setParameter(ParamType::Scene1, i, patch.getParameter(ParamType::Scene1, i), true);
    }

    for (int i = 0; i < AN1x::SceneSize; i++){
        if (AN1x::isNull(ParamType::Scene2, i)) continue;
        setParameter(ParamType::Scene2, i, patch.getParameter(ParamType::Scene2, i), true);
    }

    for (int i = 0; i < AN1x::SeqencerSize; i++) {
        if (AN1x::isNull(ParamType::StepSq, i)) continue;
        setParameter(ParamType::StepSq, i, patch.getParameter(ParamType::StepSq, i), true);
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

    auto s = PatchDatabase::getMidiSettings();

    setMidiDeviceNames(PatchDatabase::getMidiSettings().first);

}

void QAN1xEditor::setParameter(ParamType type, unsigned char param, int value, bool setAsDefault)
{
    switch (type)
    {
        case ParamType::System: setSystemParameter((AN1x::SystemParam)param, value); break;
        case ParamType::Common: setCommonParameter((AN1x::CommonParam)param, value, setAsDefault); break;
        case ParamType::Scene1: setSceneParameter((AN1x::SceneParam)param, value, false, setAsDefault); break;
        case ParamType::Scene2: setSceneParameter((AN1x::SceneParam)param, value, true, setAsDefault); break;
        case ParamType::StepSq: setSequenceParameter((AN1x::SeqParam)param, value, setAsDefault); break;
        default: break;
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

void QAN1xEditor::enableSaveButton(bool enable)
{
    ui.saveButton->setDisabled(!enable);
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

    if (p == AN1x::SystemParam::EffectBypass){

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
                ui.dlyBypass->setChecked(false);
                ui.revBypass->setChecked(true);
                break;
            case 2:
                ui.allBypass->setChecked(false);
                ui.dlyBypass->setChecked(true);
                ui.revBypass->setChecked(false);
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

    if (system_controls[p] != nullptr){

        system_controls[p]->setValue(value);
    }
}

void QAN1xEditor::setSceneParameter(AN1x::SceneParam p, int value, bool isScene2, bool setAsDefault)
{
    if (p >= AN1x::SceneSize) return;

    auto& sceneView = isScene2 ? *ui.scene2tab : *ui.scene1tab;

    sceneView.setSceneParameters(p, value, setAsDefault);
    
    auto& ctrlMatrix = isScene2 ? *ui.ctrlMatrixScene1 : *ui.ctrlMatrixScene2;

    ctrlMatrix.setSceneParameters(p, value, setAsDefault);

    ui.fxeqTab->setSceneParameter(p, value, isScene2, setAsDefault);
}

void QAN1xEditor::setCommonParameter(AN1x::CommonParam p, int value, bool setAsDefault)
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
                ui.unison->setChecked(true);
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

        if(setAsDefault) {
            ui_controls[p]->setCurrentValueAsDefault();
        }
    }

    ui.fxeqTab->setCommonParameter(p, value, setAsDefault);
    ui.seqTab->setCommonParameter(p, value, setAsDefault);
    ui.FreeEG->setCommonParameter(p, value);

}

void QAN1xEditor::setSequenceParameter(AN1x::SeqParam p, int value, bool setAsDefault)
{
    if (p >= AN1x::SeqencerSize) return;
    ui.seqTab->setSequenceParameter(p, value, setAsDefault);
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

void QAN1xEditor::setBypass()
{
    bool all = ui.allBypass->isChecked();

    if (all) {
        QSignalBlocker d(ui.dlyBypass);
        QSignalBlocker r(ui.revBypass);
        ui.dlyBypass->setChecked(true);
        ui.revBypass->setChecked(true);
        MidiMaster::parameterChanged(ParamType::System, AN1x::EffectBypass, 4);

        return;
    }

    bool dly = ui.dlyBypass->isChecked();
    bool rev = ui.revBypass->isChecked();

    bool value[4] = { (!dly && !rev),(!dly && rev),(dly && !rev),(dly && rev) };

    for (int i = 0; i < 4; i++) {
        if (value[i]) {
            MidiMaster::parameterChanged(ParamType::System, AN1x::EffectBypass, i);
        }
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

void QAN1xEditor::closeEvent(QCloseEvent* event)
{
    if (!MidiMaster::cleanUp()) {
        event->ignore();
    }

    foreach(QWidget * widget, QApplication::topLevelWidgets())
    {
        if (widget == this) continue;
        widget->close();
    }
}

PianoView* QAN1xEditor::pianoRoll()
{
    return ui.pianoView;
}

MidiDeviceNames QAN1xEditor::getCurrentDevices() const
{
    return{
        ui.inCombo->currentText().toStdString(),
        ui.outCombo->currentText().toStdString()
    };
}

void QAN1xEditor::setMidiDeviceNames(const MidiDeviceNames& devices)
{
    int in = ui.inCombo->findText(devices.midi_in.c_str());
    int out = ui.outCombo->findText(devices.midi_out.c_str());

    if (in != -1) ui.inCombo->setCurrentIndex(in);
    if (out != -1) ui.outCombo->setCurrentIndex(out);
}

bool QAN1xEditor::eventFilter(QObject* obj, QEvent* e)
{
    if (!ui.enablePcKbd->isChecked()) {
        releaseKeyboard();
        return false;
    }
/*
    static const int keyboardKeys[] = {
            Qt::Key_A,
            Qt::Key_W,
            Qt::Key_S,
            Qt::Key_E,
            Qt::Key_D,
            Qt::Key_F,
            Qt::Key_T,
            Qt::Key_G,
            Qt::Key_Y,
            Qt::Key_H,
            Qt::Key_U,
            Qt::Key_J,
            Qt::Key_K,
            Qt::Key_O,
            Qt::Key_L,
            Qt::Key_P,
            Qt::Key_Semicolon,
            Qt::Key_Apostrophe,
            Qt::Key_BracketRight,
            Qt::Key_Backslash,
            Qt::Key_X,
            Qt::Key_Z
    };
*/
    if (e->type() != QEvent::KeyPress && e->type() != QEvent::KeyRelease) {
        return QWidget::eventFilter(obj, e);
    }

    auto keyEvent = static_cast<QKeyEvent*>(e);

#ifdef Q_OS_WIN
    auto key = keyEvent->nativeVirtualKey();
#else
    auto key = keyEvent->key();
#endif

    if (e->type() == QEvent::KeyPress)
    {
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



        if (key == Qt::Key_X) {
            ui.pcKbdOctave->setValue(ui.pcKbdOctave->value() + 1);
        }
        else if (key == Qt::Key_Z) {
            ui.pcKbdOctave->setValue(ui.pcKbdOctave->value() - 1);
        }
        else {
            MidiMaster::pcKeyPress(key, true, ui.velocityKbdSpin->value());
        }

        return false;
    }

    if (e->type() == QEvent::KeyRelease)
    {
        
        if (keyEvent->modifiers()) goto here;

        if (keyEvent->isAutoRepeat()) goto here;

        MidiMaster::pcKeyPress(key, false, ui.velocityKbdSpin->value());

        return false;
    }

here:

    return QWidget::eventFilter(obj, e);
 
}


QAN1xEditor::~QAN1xEditor()
{}
