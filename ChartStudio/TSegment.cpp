#include "stdafx.h"
#include "TSegment.h"
#include "TPoint.h"
#include "TRect.h"
#include "helpers.h"


TSegment::TSegment(TPoint point1, TPoint point2) {
	this->point1 = point1;
	this->point2 = point2;
}

TSegment::~TSegment() {
}

bool TSegment::getSegmentIntersection(TPoint& returnPoint, TSegment seg1, TSegment seg2, bool needsToBeTouching) {
	TPoint xy(0.0f, 0.0f);
	double t = (seg2.point2.y - seg2.point1.y)*(seg1.point2.x - seg1.point1.x) - (seg2.point2.x - seg2.point1.x)*(seg1.point2.y - seg1.point1.y);
	if (!compare_float(t, 0.0f)) {
		double ua = ((seg2.point2.x - seg2.point1.x)*(seg1.point1.y - seg2.point1.y) - (seg2.point2.y - seg2.point1.y)*(seg1.point1.x - seg2.point1.x)) / t;
		xy.x = seg1.point1.x + ua * (seg1.point2.x - seg1.point1.x);
		xy.y = seg1.point1.y + ua * (seg1.point2.y - seg1.point1.y);
		
		if (needsToBeTouching) {
			TRect rect1(seg1.point1, seg1.point2);
			TRect rect2(seg2.point1, seg2.point2);
			if (rect1.isPointInside(xy) && rect2.isPointInside(xy)) {
				returnPoint = xy;
				return true;
			}
		} else {
			returnPoint = xy;
			return true;
		}
	}
	return false;
}

bool TSegment::getSegmentIntersection(TPoint& returnPoint, TSegment seg2, bool needsToBeTouching) {
	return getSegmentIntersection(returnPoint, *this, seg2, needsToBeTouching);
}

bool TSegment::figureYCrossing(double& returnValue, TSegment seg1, double xAxis) {
	TSegment seg2(TPoint(xAxis, -100), TPoint(xAxis, 100));
	TPoint intersection;
	if (getSegmentIntersection(intersection, seg1, seg2, false)) {
		returnValue = intersection.y;
		return true;
	}
	return false;
}

bool TSegment::figureYCrossing(double& returnValue, double xAxis) {
	return figureYCrossing(returnValue, *this, xAxis);
}

TSegment TSegment::getOppositeSegmentAt(TPoint atPoint) {
	TPoint otherPoint(atPoint.x + this->point1.y - this->point2.y, atPoint.y + this->point2.x - this->point1.x);
	return TSegment(atPoint,otherPoint);
}