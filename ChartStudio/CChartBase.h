#pragma once
#include <string>
#include <vector>
#include "TRect.h"
#include "CChartObject.h"
#include "helpers.h"
using namespace std;

class TLine;
class TPoint;
class ChartImage;

struct BoundSettings {
	double leftPosition;
	double leftValue;
	double topPosition;
	double topValue;
	double rightPosition;
	double rightValue;
	double bottomPosition;
	double bottomValue;

	BoundSettings(double leftPosition, double leftValue, double topPosition, double topValue, double rightPosition, double rightValue, double bottomPosition, double bottomValue) {
		this->leftPosition = leftPosition;
		this->leftValue = leftValue;
		this->topPosition = topPosition;
		this->topValue = topValue;
		this->rightPosition = rightPosition;
		this->rightValue = rightValue;
		this->bottomPosition = bottomPosition;
		this->bottomValue = bottomValue;
	}

	BoundSettings() {
		this->leftPosition = 0;
		this->leftValue = 0;
		this->topPosition = 0;
		this->topValue = 0;
		this->rightPosition = 10;
		this->rightValue = 0;
		this->bottomPosition = 10;
		this->bottomValue = 0;
	}

	string toString() {
		string retString= "(" + trimmedDoubleToString(leftPosition) + "," + trimmedDoubleToString(leftValue) + ",";
		retString+= trimmedDoubleToString(topPosition) + "," + trimmedDoubleToString(topValue) + ",";
		retString+= trimmedDoubleToString(rightPosition) + "," + trimmedDoubleToString(rightValue) + ",";
		retString += trimmedDoubleToString(bottomPosition) + "," + trimmedDoubleToString(bottomValue) + ")";
		return retString;
	}
};


class CChartBase : public CChartObject {
public:
	CChartBase(string name, ChartObjectType type, bool exportResult, bool yIsInputAxis, BoundSettings bounds, vector<TLine*> lines);
	CChartBase(string name, ChartObjectType type, bool exportResult, bool yIsInputAxis, BoundSettings bounds, string inputVar1Link, string inputVar2Link, vector<TLine*> lines);
	virtual ~CChartBase() = 0;

	string inputVariable1Link;
	string inputVariable2Link;

	CChartObject* inputVariable1 = nullptr;
	CChartObject* inputVariable2 = nullptr;
	vector<TLine*> lines;

	vector<TLine*> calcLines;
	bool areTwoCornersInRect(TRect rect);
	void screenToChart(TPoint* point);
	void chartToScreen(TPoint * point);
	//void setLinesToAbsolute();
	//bool setLinesToRelative();
	virtual void setCalcLines();
	virtual void deleteCalcLines();

	bool yIsInputAxis;
	BoundSettings bounds = BoundSettings(0,0,0,0,0,0,0,0);

	virtual void deleteLine(TLine * line);
	TRect boundPosToRect();
	virtual void sortLines();
	virtual TLine* addLine(double value);
	virtual void addLine(TLine * line);
	virtual void setLineValue(TLine* line, double value);
	virtual double getLineValue(TLine* line);
};

