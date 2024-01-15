#pragma once

#include <QWidget>
#include "ui_FreeEGWidget.h"

class FreeEGScene;

class FreeEGWidget : public QWidget
{
	Q_OBJECT

	static constexpr int ui_size = AN1x::CommonParam::FreeEgData - AN1x::CommonParam::FreeEGTrigger;

	std::array<AbstractController*, ui_size> ui_controls{ nullptr };

	FreeEGScene* scene;

public:
	FreeEGWidget(QWidget *parent = nullptr);
	~FreeEGWidget();

	void setCommonParameter(AN1x::CommonParam p, int value);

	std::vector<int> getFreeEGData();
	void setTrackData(const std::vector<int>& data);

	Ui::FreeEGWidgetClass ui;
};
