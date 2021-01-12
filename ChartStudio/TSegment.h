#pragma once
#include "TPoint.h"

class TSegment {
public:
	TSegment(TPoint point1 = TPoint(), TPoint point2 = TPoint());
	~TSegment();

	bool getSegmentIntersection(TPoint& returnPoint,TSegment seg1, TSegment seg2, bool needsToBeTouching);
	bool getSegmentIntersection(TPoint& returnPoint, TSegment seg2, bool needsToBeTouching);

	bool figureYCrossing(double& returnValue, TSegment seg1, double xAxis);
	bool figureYCrossing(double& returnValue, double xAxis);

	TSegment getOppositeSegmentAt(TPoint atPoint);

	TPoint point1;
	TPoint point2;
};

