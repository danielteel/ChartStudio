#include "stdafx.h"
#include "TRect.h"
#include "TPoint.h"

TRect::TRect(TPoint point1, TPoint point2) {
	this->point1 = point1;
	this->point2 = point2;
}


TRect::~TRect() {
}

bool TRect::isPointInside(TPoint point) {
	return ((point.x >= point1.x && point.x <= point2.x) || (point.x >= point2.x && point.x <= point1.x)) &&
		((point.y >= point1.y && point.y <= point2.y) || (point.y >= point2.y && point.y <= point1.y));
}

bool TRect::isPointInside(TPoint point, double padding) {
	TPoint a = this->point1;
	TPoint b = this->point2;

	if (a.x < b.x) {
		a.x -= padding;
		b.x += padding;
	} else {
		a.x += padding;
		b.x -= padding;
	}

	if (a.y < b.y) {
		a.y -= padding;
		b.y += padding;
	} else {
		a.y += padding;
		b.y -= padding;
	}

	TRect rect(a, b);

	return rect.isPointInside(point);
}