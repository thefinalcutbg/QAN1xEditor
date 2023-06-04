#pragma once
#include "Model/AN1x.h"
#include <qobject.h>

class MidiMaster;

class AbstractSceneController
{
protected:
	int defaultValue{ 0 };
public:
	virtual void setCurrentValueAsDefault() = 0;
	virtual void setValue(int value) = 0;
	virtual void setSceneParam(MidiMaster* m, AN1x::SceneParam p, bool isScene2) {};
	virtual void setCommonParam(MidiMaster* m, AN1x::CommonParam p) {};
};

