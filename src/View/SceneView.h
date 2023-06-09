#pragma once

#include <QWidget>
#include "ui_SceneView.h"
#include "Model/An1x.h"

class SceneView : public QWidget
{
	Q_OBJECT
	
	bool isScene2 = false;

	static constexpr int uiParamSize = AN1x::VariFxDW;

	std::array<AbstractController*, uiParamSize> ui_controls { nullptr };

	AN1x::ParamType getType() {
		return isScene2 ? AN1x::ParamType::Scene2 : AN1x::ParamType::Scene1;
	}

public:
	SceneView(QWidget *parent = nullptr);

	void setAsScene2() { isScene2 = true; }

	void setSceneParameters(AN1x::SceneParam p, int value);

	~SceneView();

private:
	Ui::SceneViewClass ui;
};
