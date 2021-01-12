#include "stdafx.h"
#include "ChartImage.h"
#include "TLine.h"
#include "TPoint.h"
#include "TRect.h"
#include "CChartBase.h"


CChartBase::CChartBase(string name, ChartObjectType type, bool exportResult, bool yIsInputAxis, BoundSettings bounds, vector<TLine*> lines) : CChartObject(name, type, exportResult) {
	this->yIsInputAxis = yIsInputAxis;
	this->bounds = bounds;
	this->lines = lines;
}

CChartBase::CChartBase(string name, ChartObjectType type, bool exportResult, bool yIsInputAxis, BoundSettings bounds, string input1Link, string input2Link, vector<TLine*> lines) : CChartObject(name, type, exportResult) {
	this->yIsInputAxis = yIsInputAxis;
	this->bounds = bounds;
	this->lines = lines;
	this->inputVariable1Link = input1Link;
	this->inputVariable2Link = input2Link;
}

CChartBase::~CChartBase() {
	for (auto & currentLine : this->lines) {
		delete currentLine;
	}
	deleteCalcLines();
}


bool CChartBase::areTwoCornersInRect(TRect rect) {
	TPoint ul(this->bounds.leftPosition, this->bounds.topPosition);
	TPoint br(this->bounds.rightPosition, this->bounds.bottomPosition);
	TPoint ur(this->bounds.rightPosition, this->bounds.topPosition);
	TPoint bl(this->bounds.leftPosition, this->bounds.bottomPosition);

	int cornersIn = 0;
	if (rect.isPointInside(ul)) cornersIn++;
	if (rect.isPointInside(br)) cornersIn++;
	if (rect.isPointInside(ur)) cornersIn++;
	if (rect.isPointInside(bl)) cornersIn++;

	if (cornersIn>=2) {
		return true;
	}
	return false;
}

void CChartBase::screenToChart(TPoint* point) {
	double xValDiff = this->bounds.rightValue - this->bounds.leftValue;
	double yValDiff = this->bounds.bottomValue - this->bounds.topValue;
	double xDiff = this->bounds.rightPosition - this->bounds.leftPosition;
	double yDiff = this->bounds.bottomPosition - this->bounds.topPosition;
	if (!compare_float(xDiff, 0) && !compare_float(yDiff, 0)) {
		point->x = ((point->x - this->bounds.leftPosition) / xDiff) * xValDiff + this->bounds.leftValue;
		point->y = ((point->y - this->bounds.topPosition) / yDiff) * yValDiff + this->bounds.topValue;
	} else {
		point->x = 0;
		point->y = 0;
	}
}

void CChartBase::chartToScreen(TPoint* point) {
	double xValDiff = this->bounds.rightValue - this->bounds.leftValue;
	double yValDiff = this->bounds.bottomValue - this->bounds.topValue;
	double xDiff = this->bounds.rightPosition - this->bounds.leftPosition;
	double yDiff = this->bounds.bottomPosition - this->bounds.topPosition;
	if (!compare_float(xValDiff, 0) && !compare_float(yValDiff, 0)) {
		point->x = ((point->x - this->bounds.leftValue) / xValDiff) * xDiff + this->bounds.leftPosition;
		point->y = ((point->y - this->bounds.topValue) / yValDiff) * yDiff + this->bounds.topPosition;
	} else {
		point->x = 0;
		point->y = 0;
	}
}

/*
void CChartBase::setLinesToAbsolute() {
	for (auto & line : this->lines) {
		for (auto & point : line->points) {
			this->chartToScreen(point);
		}
	}
}

bool CChartBase::setLinesToRelative() {
	double xDiff = this->boundSettings.rightPosition - this->boundSettings.leftPosition;
	double yDiff = this->boundSettings.bottomPosition - this->boundSettings.topPosition;
	double xValDiff = this->boundSettings.rightValue - this->boundSettings.leftValue;
	double yValDiff = this->boundSettings.bottomValue - this->boundSettings.topValue;
	if (!compare_float(xDiff, 0, .005f) && !compare_float(yDiff, 0, .005f) && !compare_float(xValDiff, 0, 1.0f) && !compare_float(yValDiff, 0, 1.0f)) {
		for (auto & line : this->lines) {
			for (auto & point : line->points) {
				this->screenToChart(point);
			}
		}
		return true;
	}
	return false;
}
*/

void CChartBase::setCalcLines() {
	deleteCalcLines();

	for (auto & line : this->lines) {
		TLine* lineCopy = line->copyLine();
		if (lineCopy) {
			for (auto & point : lineCopy->points) {
				this->screenToChart(point);
			}

			this->calcLines.push_back(lineCopy);
		}
	}
}

void CChartBase::deleteCalcLines() {
	if (this->calcLines.empty()) return;
	for (auto & calcLine : this->calcLines) {
		delete calcLine;
	}
	this->calcLines.clear();
}

void CChartBase::deleteLine(TLine* line) {
	for (auto it = this->lines.begin(); it != this->lines.end(); it++) {
		if (*it == line) {
			delete *it;
			it = this->lines.erase(it);
			break;
		}
	}
}

TRect CChartBase::boundPosToRect() {
	return TRect(TPoint(bounds.leftPosition, bounds.topPosition), TPoint(bounds.rightPosition, bounds.bottomPosition));
}

void CChartBase::sortLines() {
	if (this->lines.size() < 2) return;

	bool notSwapped;
	do {
		notSwapped = true;
		for (size_t i = 0; i < this->lines.size() - 1; i++) {
			if (this->lines[i]->getValue() > this->lines[i + 1]->getValue()) {
				TLine* swapper = this->lines[i];
				this->lines[i] = this->lines[i + 1];
				this->lines[i + 1] = swapper;
				notSwapped = false;
			}
		}
	} while (notSwapped == false);
}

TLine* CChartBase::addLine(double value = 0.0f) {
	TLine* newLine = new TLine(value);
	this->lines.push_back(newLine);
	this->sortLines();
	return newLine;
}

void CChartBase::addLine(TLine* line) {
	if (line) {
		this->lines.push_back(line);
		this->sortLines();
	}
}

void CChartBase::setLineValue(TLine * line, double value) {
	if (line) {
		line->value = value;
		this->sortLines();
	}
}

double CChartBase::getLineValue(TLine * line) {
	if (line) {
		return line->value;
	}
	return 0;
}
