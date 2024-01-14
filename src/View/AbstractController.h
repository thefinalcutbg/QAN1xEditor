#pragma once
#include "Model/AN1x.h"

class AbstractController
{
protected:
	
	ParamType type = ParamType::Unknown;
	unsigned char parameter;
	int defaultValue{ 0 };
public:

	void setParam(ParamType t, unsigned char p) {
		this->type = t;
		this->parameter = p;
	};

	virtual void setCurrentValueAsDefault() = 0;
	virtual void setValue(int value) = 0;
	virtual int getValue() = 0;
};

