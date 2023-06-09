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

public:
	SceneView(QWidget *parent = nullptr);

	void setAsScene(bool isSceen2);

	void setSceneParameters(AN1x::SceneParam p, int value);

	~SceneView();

private:
	Ui::SceneViewClass ui;
};
