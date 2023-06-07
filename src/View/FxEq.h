#pragma once

#include <QWidget>
#include "ui_FxEq.h"
#include "Model/MidiMaster.h"
#include <array>

class FxEq : public QWidget
{
	Q_OBJECT

	static constexpr int ui_size = AN1x::NullCommon22 - AN1x::VariFXType;

	std::array<AbstractSceneController*, ui_size> ui_controls{ nullptr };

	void setFxLayout(int value);
	void setDelayLayout(int value);
	void setReverbLayout();
	void setEqLayout();

public:
	FxEq(QWidget *parent = nullptr);

	void setCommonParameter(AN1x::CommonParam p, int value);
	void setSceneParameter(AN1x::SceneParam p, int value, bool isScene2);
	void setMidiMaster(MidiMaster* m);

	~FxEq();

private:
	Ui::FxEq ui;
};
