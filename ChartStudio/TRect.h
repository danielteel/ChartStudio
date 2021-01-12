#pragma once
#include "TPoint.h"

class TRect {
public:
	TRect(TPoint point1, TPoint point2);
	~TRect();

	bool isPointInside(TPoint point);

	bool isPointInside(TPoint point, double padding);

	TPoint point1;
	TPoint point2;
};

