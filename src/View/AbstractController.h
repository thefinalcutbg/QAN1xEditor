#pragma once
#include "Model/AN1x.h"

class AbstractController
{
protected:
	
	AN1x::ParamType type = AN1x::ParamType::Unknown;
	unsigned char parameter;
	int defaultValue{ 0 };
public:

	void setParam(AN1x::ParamType t, unsigned char p) {
		this->type = t;
		this->parameter = p;
	};

	virtual void setCurrentValueAsDefault() = 0;
	virtual void setValue(int value) = 0;
};

