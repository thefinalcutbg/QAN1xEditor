#include "SceneView.h"
#include "Model/MidiMaster.h"
#include "GlobalWidgets.h"
#include <QPainter>

SceneView::SceneView(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

}

void SceneView::setSceneParameters(AN1x::SceneParam p, int value)
{
	if (p == AN1x::PolyMode) {
		
		QSignalBlocker arr[] = { 
			QSignalBlocker{ui.polyRadio}, 
			QSignalBlocker{ui.monoRadio}, 
			QSignalBlocker{ui.legatoRadio } 
		};
		
		ui.polyRadio->setChecked(value == 0);
		ui.monoRadio->setChecked(value == 1);
		ui.legatoRadio->setChecked(value == 2);

		if (value == 0) {
			ui.portamentoType->setItemText(0, "Normal");
			ui.portamentoType->setItemText(1, "Sustain-key");
		}
		else
		{
			ui.portamentoType->setItemText(0, "Full-time");
			ui.portamentoType->setItemText(1, "Fingered");
		}

		return;
	}

	if (p == AN1x::LFOResetMode) {

		QSignalBlocker b(ui.lfoReset);
		ui.lfoReset->setChecked(value);
		return;
	}

	if (p < ui_controls.size() && ui_controls[p] != nullptr) {
		ui_controls[p]->setValue(value);
	}
}

void SceneView::setMidiMaster(MidiMaster* master)
{
	m = master;

	connect(ui.polyRadio, &QRadioButton::clicked, [=] { 
		master->setSceneParam(AN1x::SceneParam::PolyMode, 0, isScene2);
		ui.portamentoType->setItemText(0, "Normal");
		ui.portamentoType->setItemText(1, "Sustain-key"); 
	});

	connect(ui.monoRadio, &QRadioButton::clicked, [=] { 
		master->setSceneParam(AN1x::SceneParam::PolyMode, 1, isScene2); 
		ui.portamentoType->setItemText(0, "Full-time"); 
		ui.portamentoType->setItemText(1, "Fingered"); 
		});

	connect(ui.legatoRadio, &QRadioButton::clicked, [=] { 
		master->setSceneParam(AN1x::SceneParam::PolyMode, 2, isScene2); 
		ui.portamentoType->setItemText(0, "Full-time"); 
		ui.portamentoType->setItemText(1, "Fingered"); 
	});

	ui_controls = {
		nullptr,
		nullptr,
		nullptr,
		ui.pegDecay,
		ui.pegDepth,
		ui.pegSwitch,
		ui.portamentoType,
		ui.portamentoTime,
		nullptr,
		ui.LFOShape,
		ui.lfo1Speed,
		nullptr,
		ui.lfo1Delay,
		ui.lfo2Speed,
		nullptr,
		ui.algorithmCombo,
		ui.syncPitch,
		ui.syncDepth,
		ui.syncPitchSrc,
		ui.syncModSwitch,
		ui.FMDepth,
		ui.FM1srcCombo,
		ui.FM2srcCombo,
		ui.osc1wave,
		ui.pitch1,
		ui.fine1,
		ui.edge1,
		ui.PW1,
		ui.depth1,
		ui.PWSrc1,
		ui.VCO1PitchModDepth,
		nullptr,
		ui.osc2wave,
		ui.pitch2,
		ui.fine2,
		ui.edge2,
		ui.PW2,
		ui.depth2,
		ui.PWSrc2,
		ui.VCO2PitchModDepth,
		nullptr,
		ui.mixVCO1,
		ui.mixVCO2,
		ui.mixRingMod,
		ui.mixNoise,
		ui.filterAttack,
		ui.filterDecay,
		ui.filterSustain,
		ui.filterRelease,
		ui.hpf,
		ui.filterType,
		ui.filterCutoff,
		ui.filterResonance,
		ui.filterDepth,
		nullptr,
		ui.filterVelSens,
		ui.filterKeyTrk,
		ui.FilterModDepth,
		ui.ampAttack,
		ui.ampDecay,
		ui.ampSustain,
		ui.ampRelease,
		ui.feedback,
		ui.ampVolume,
		ui.ampVelSens,
		ui.AmpModDepth
	};

	for (int i = 0; i < ui_controls.size(); i++)
	{
		if (ui_controls[i] == nullptr) continue;

		ui_controls[i]->setCurrentValueAsDefault();
		ui_controls[i]->setSceneParam(m, (AN1x::SceneParam)i, isScene2);
	}
	
	connect(ui.lfoReset, &QCheckBox::clicked, [=](bool checked) { master->setSceneParam(AN1x::SceneParam::LFOResetMode, checked, isScene2); });
	

	connect(ui.algorithmCombo, &QComboBox::currentIndexChanged, 
		[=](int idx) {

			ui.syncGroup->setEnabled(idx);

			if (idx == 0 && ui.osc1wave->count() == 5) {

				ui.osc1wave->setItemText(2, "Saw 2");
				ui.osc1wave->setItemText(3, "Mix");
				ui.osc1wave->removeItem(4);
				return;
			}
			else if (idx != 0 && ui.osc1wave->count() == 4) {

				ui.osc1wave->setItemText(2, "Inner1");
				ui.osc1wave->setItemText(3, "Inner2");
				ui.osc1wave->addItem("Inner3");
				return;
			}
				
		});
};


SceneView::~SceneView()
{}
