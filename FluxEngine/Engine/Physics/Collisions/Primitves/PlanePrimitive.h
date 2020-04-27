#pragma once
#include "Primitive.h"
class PlanePrimitive : public Primitive
{
public:
	XMVECTOR normal;
	float offset;
};

