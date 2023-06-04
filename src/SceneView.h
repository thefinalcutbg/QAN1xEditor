#pragma once

#include <QWidget>
#include "ui_SceneView.h"

class MidiMaster;

class SceneView : public QWidget
{
	Q_OBJECT

	MidiMaster* m{ nullptr };
	
	bool isScene2 = false;

	std::array<AbstractSceneController*, AN1x::SceneParamSize> ui_controls { nullptr };

public:
	SceneView(QWidget *parent = nullptr);

	void setAsScene2() { isScene2 = true; }

	void setSceneParameters(AN1x::SceneParam p, int value);

	void setMidiMaster(MidiMaster* master);

	~SceneView();

private:
	Ui::SceneViewClass ui;
};
