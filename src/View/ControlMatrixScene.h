#pragma once

#include <QWidget>
#include "ui_ControlMatrixScene.h"

class ControlMatrixScene : public QWidget
{
	Q_OBJECT

	bool isScene2 = false;

	static constexpr int uiControlsSize = 3 * 16;

	std::array<AbstractController*, uiControlsSize> ui_controls{ nullptr };

	AN1x::ParamType getType() {
		return isScene2 ? AN1x::ParamType::Scene1 : AN1x::ParamType::Scene2;
	}

public:
	ControlMatrixScene(QWidget* parent = nullptr);

	void setAsScene2() { isScene2 = true; }

	void setSceneParameters(AN1x::SceneParam p, int value);


	~ControlMatrixScene();

private:
	Ui::ControlMatrixSceneClass ui;
};
