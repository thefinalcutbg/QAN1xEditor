#pragma once

#include <QWidget>
#include "ui_ControlMatrixScene.h"

class ControlMatrixScene : public QWidget
{
	Q_OBJECT

	bool isScene2 = false;

	static constexpr int uiControlsSize = 3 * 16;

	std::array<AbstractController*, uiControlsSize> ui_controls{ nullptr };

public:
	ControlMatrixScene(QWidget* parent = nullptr);

	void setAsScene(bool isScene2);

	void setSceneParameters(AN1x::SceneParam p, int value);


	~ControlMatrixScene();

private:
	Ui::ControlMatrixSceneClass ui;
};
