#pragma once

#include <QWidget>
#include "ui_ControlMatrixScene.h"

class ControlMatrixScene : public QWidget
{
	Q_OBJECT

	bool isScene2 = false;

	static constexpr int uiControlsSize = 3 * 16;

	std::array<AbstractSceneController*, uiControlsSize> ui_controls{ nullptr };

public:
	ControlMatrixScene(QWidget* parent = nullptr);

	void setAsScene2() { isScene2 = true; }

	void setSceneParameters(AN1x::SceneParam p, int value);

	void setMidiMaster(MidiMaster* master);

	~ControlMatrixScene();

private:
	Ui::ControlMatrixSceneClass ui;
};
