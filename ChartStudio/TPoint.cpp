#include "stdafx.h"
#include <math.h>
#include "TPoint.h"
#include "helpers.h"


TPoint::TPoint(double x, double y) {
	this->x = x;
	this->y = y;
}

TPoint::~TPoint() {
}

void TPoint::flip() {
	double oldX = this->x;
	this->x = this->y;
	this->y = oldX;
}


double TPoint::distance(TPoint toPoint) {
	double xDiff = fabs(toPoint.x - this->x);
	double yDiff = fabs(toPoint.y - this->y);
	return sqrt(xDiff*xDiff + yDiff*yDiff);
}

string TPoint::toString() {
	return "(" + trimmedDoubleToString(x) + "," + trimmedDoubleToString(y) + ")";
}
