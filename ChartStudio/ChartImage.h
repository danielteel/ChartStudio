#pragma once
#include <string>
#include "TPoint.h"
using namespace std;

class TPoint;
class TRect;

class ChartImage {
public:
	string name;

	TRect getRect();

	ChartImage(string name, HBITMAP bitmap, TPoint upperLeft);

	~ChartImage();

	TPoint getPosition();

	TPoint getSize();

	void move(TPoint newXY, TPoint newSize);

	void setHBitmap(HBITMAP newBitmap);

	HBITMAP getHBitmap();

	bool isLess(const ChartImage* than);

	string toString(char** imageData, size_t* imageDataLen, ULONG quality);

private:
	HBITMAP bitmap;

	TPoint position;
	TPoint size;

};