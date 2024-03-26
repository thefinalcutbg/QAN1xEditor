#include "ControlMatrixScene.h"

ControlMatrixScene::ControlMatrixScene(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);


}

void ControlMatrixScene::setAsScene(bool isScene2)
{
	ParamType type = isScene2 ? ParamType::Scene2 : ParamType::Scene1;

	ui_controls = {
	ui.source_1,
	ui.param_1,
	ui.depth_1,
	ui.source_2,
	ui.param_2,
	ui.depth_2,
	ui.source_3,
	ui.param_3,
	ui.depth_3,
	ui.source_4,
	ui.param_4,
	ui.depth_4,
	ui.source_5,
	ui.param_5,
	ui.depth_5,
	ui.source_6,
	ui.param_6,
	ui.depth_6,
	ui.source_7,
	ui.param_7,
	ui.depth_7,
	ui.source_8,
	ui.param_8,
	ui.depth_8,
	ui.source_9,
	ui.param_9,
	ui.depth_9,
	ui.source_10,
	ui.param_10,
	ui.depth_10,
	ui.source_11,
	ui.param_11,
	ui.depth_11,
	ui.source_12,
	ui.param_12,
	ui.depth_12,
	ui.source_13,
	ui.param_13,
	ui.depth_13,
	ui.source_14,
	ui.param_14,
	ui.depth_14,
	ui.source_15,
	ui.param_15,
	ui.depth_15,
	ui.source_16,
	ui.param_16,
	ui.depth_16
	};

    for (size_t i = 0; i < ui_controls.size(); i++)
	{
		auto combo = static_cast<ComboPicker*>(ui_controls[i]);

		for (int y = 0; y < 115; y++)
		{
			combo->addItem(AN1x::getMatrixSourceName(y).c_str());
		}

		auto param = static_cast<ComboPicker*>(ui_controls[++i]);

		for (int y = 0; y < 46; y++)
		{
			param->addItem(AN1x::getMatrixParamName(y));
		}

		auto dial = static_cast<DialKnob*>(ui_controls[++i]);
		dial->setRange(0, 127);
		dial->setOffset(-64);
		dial->setValue(64);
		dial->showPlusOnPositives(true);
		dial->setCurrentValueAsDefault();
	}

    for (size_t i = 0; i < ui_controls.size(); i++)
	{
		auto parameter = static_cast<int>(AN1x::CtrlMtrxSource1) + i;

		ui_controls[i]->setCurrentValueAsDefault();
		ui_controls[i]->setParam(type, (AN1x::SceneParam)parameter);
	}

}

void ControlMatrixScene::setSceneParameters(AN1x::SceneParam p, int value, bool setAsDefault)
{
	if (p < AN1x::CtrlMtrxSource1 || p > AN1x::SceneSize) return;

    auto ctrl = ui_controls[p - AN1x::CtrlMtrxSource1];

    ctrl->setValue(value);

    if(setAsDefault){
        ctrl->setCurrentValueAsDefault();
    }
}

ControlMatrixScene::~ControlMatrixScene()
{}
