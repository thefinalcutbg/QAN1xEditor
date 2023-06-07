#pragma once

#include <QWidget>
#include "ui_SceneView.h"
#include "Model/An1x.h"

class MidiMaster;

class SceneView : public QWidget
{
	Q_OBJECT

	MidiMaster* m{ nullptr };
	
	bool isScene2 = false;

	static constexpr int uiParamSize = AN1x::VariFxDW;

	std::array<AbstractSceneController*, uiParamSize> ui_controls { nullptr };

public:
	SceneView(QWidget *parent = nullptr);

	void setAsScene2() { isScene2 = true; }

	void setSceneParameters(AN1x::SceneParam p, int value);

	void setMidiMaster(MidiMaster* master);

	~SceneView();

private:
	Ui::SceneViewClass ui;
};
