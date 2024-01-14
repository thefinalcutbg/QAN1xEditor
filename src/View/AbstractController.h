#pragma once
#include "Model/AN1x.h"

class AbstractController
{
protected:
	
	AN1xParam::Type type = AN1xParam::Type::Unknown;
	unsigned char parameter;
	int defaultValue{ 0 };
public:

	void setParam(AN1xParam::Type t, unsigned char p) {
		this->type = t;
		this->parameter = p;
	};

	virtual void setCurrentValueAsDefault() = 0;
	virtual void setValue(int value) = 0;
	virtual int getValue() = 0;
};

