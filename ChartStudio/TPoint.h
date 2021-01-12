#pragma once
#include <string>
using namespace std;
class TPoint {
public:
	TPoint(double x = 0, double y = 0);
	~TPoint();

	void flip();
	double distance(TPoint toPoint);
	double x;
	double y;

	TPoint operator+(const TPoint& b) {
		return TPoint(this->x + b.x, this->y + b.y);
	}

	TPoint operator-(const TPoint& b) {
		return TPoint(this->x - b.x, this->y - b.y);
	}

	string toString();
};

