#include <algorithm>
#include <vector>
#include <string>
#include "stdafx.h"
#include "helpers.h"
#include "TLine.h"
#include "TPoint.h"
#include "TSegment.h"
#include "CLinearChart.h"


CLinearChart::CLinearChart(string name, bool exportResult, bool yIsInputAxis, BoundSettings bounds, vector<TLine*> lines) : CChartBase(name, ChartObjectType::LinearChart, exportResult, yIsInputAxis, bounds, lines) {
}

CLinearChart::CLinearChart(string name, bool exportResult, bool yIsInputAxis, BoundSettings bounds, string in1Link, string in2Link, vector<TLine*> lines) : CChartBase(name, ChartObjectType::LinearChart, exportResult, yIsInputAxis, bounds, in1Link, in2Link, lines) {
}

CLinearChart::~CLinearChart() {
}

bool CLinearChart::checkIsBad(ChartProject* chartProject, string& whyItsBad) {
	bool isBad = false;
	whyItsBad = "";
	if (this->inputVariable1 == nullptr || this->inputVariable2 == nullptr) {
		whyItsBad += "Empty input(s);\r\n";
		isBad = true;
	}

	if (this->lines.size() == 0) {
		whyItsBad += "No lines;\r\n";
		isBad = true;
	}
	for (auto & line : this->lines) {
		if (line->points.size() < 2) {
			whyItsBad += "Line(s) has less then 2 points;\r\n";
			isBad = true;
			break;
		}
		if (line->isLinear(this->yIsInputAxis) == false) {
			whyItsBad += "Line(s) not linear;\r\n";
			isBad = true;
			break;
		}
	}
	if (isBad == false) {
		whyItsBad = "Good";
	} else {
		whyItsBad = whyItsBad.substr(0, whyItsBad.length() - 2);//trim last \r\n
	}
	return isBad;
}


bool CLinearChart::getRelevantLines(vector<TLine*>& lines, double z, TLine*& belowLine, TLine*& aboveLine) {
	for (auto& currentLine : lines) {
		if (compare_float(currentLine->getValue(), z)) {
			belowLine = currentLine;
			aboveLine = currentLine;
			break;
		}
		if (currentLine->getValue() < z) {
			if (belowLine != nullptr) {
				if (belowLine->getValue() < currentLine->getValue()) {
					belowLine = currentLine;
				}
			}
			else {
				belowLine = currentLine;
			}
		}
		if (currentLine->getValue() > z) {
			if (aboveLine != nullptr) {
				if (aboveLine->getValue() > currentLine->getValue()) {
					aboveLine = currentLine;
				}
			}
			else {
				aboveLine = currentLine;
			}
		}
	}
	if (aboveLine == nullptr || belowLine == nullptr) {
		return false;
	}
	return true;
}

bool comparePointsY(TPoint* a, TPoint* b) {
	if (a && b) {
		return (a->y < b->y);
	}
	return false;
}

bool comparePointsX(TPoint* a, TPoint* b) {
	if (a && b) {
		return (a->x < b->x);
	}
	return false;
}

bool CLinearChart::autoGenerateLine(double z, TLine** createdLine)
{
	TLine* belowLine = nullptr;
	TLine* aboveLine = nullptr;


	bool foundTheLines = this->getRelevantLines(this->lines, z, belowLine, aboveLine);

	if (foundTheLines == false) {
		return false;
	}

	if (belowLine == aboveLine) {
		return false;//need two different lines to interpolate between for new line generation
	}

	belowLine = belowLine->copyLine();
	aboveLine = aboveLine->copyLine();

	if (this->yIsInputAxis) {//Flip points if y is the input axis
		aboveLine->flipPoints();
		belowLine->flipPoints();
	}

	if (aboveLine->isLinear(false) == false || belowLine->isLinear(false) == false) {
		delete belowLine, aboveLine;
		return false;
	}

	TLine* finalLine = nullptr;
	finalLine = new TLine();

	for (auto& point : belowLine->points) {
		if (this->reverseInterpolate) {
			finalLine->addNode(new TPoint(0, point->y));
		}else{
			finalLine->addNode(new TPoint(point->x, 0));
		}
	}
	for (auto& point : aboveLine->points) {
		bool hasThisAlready = false;
		for (auto& existingPoint : finalLine->points) {
			if (this->reverseInterpolate) {
				if (compare_float(existingPoint->y, point->y)) {
					hasThisAlready = true;
				}
			}else {
				if (compare_float(existingPoint->x, point->x)) {
					hasThisAlready = true;
				}
			}
		}
		if (hasThisAlready == false) {
			if (this->reverseInterpolate) {
				finalLine->addNode(new TPoint(0, point->y));
			}
			else {
				finalLine->addNode(new TPoint(point->x, 0));
			}
		}
	}

	sort(finalLine->points.begin(), finalLine->points.end(), this->reverseInterpolate ? comparePointsY : comparePointsX);

	if (this->reverseInterpolate) {
		aboveLine->flipPoints();
		belowLine->flipPoints();
	}

	for (auto& point : finalLine->points) {
		TSegment aboveSegment;
		TSegment belowSegment;
		if (aboveLine->findSegment(aboveSegment, this->reverseInterpolate ? point->y : point->x) && belowLine->findSegment(belowSegment, this->reverseInterpolate ? point->y : point->x)) {
			double abovePos, belowPos;
			if (aboveSegment.figureYCrossing(abovePos, this->reverseInterpolate ? point->y : point->x) && belowSegment.figureYCrossing(belowPos, this->reverseInterpolate ? point->y : point->x)) {
				double posDiff = abovePos - belowPos;
				double valDiff = aboveLine->getValue() - belowLine->getValue();
				if (this->reverseInterpolate) {
					point->x = (z - belowLine->getValue()) / valDiff * posDiff + belowPos;
				}else {
					point->y = (z - belowLine->getValue()) / valDiff * posDiff + belowPos;
				}
			}else {
				delete finalLine, belowLine, aboveLine;
				return false;
			}
		}else {
			delete finalLine, belowLine, aboveLine;
			return false;
		}
	}
	if (this->yIsInputAxis) {
		finalLine->flipPoints();
	}
	this->addLine(finalLine);
	this->setLineValue(finalLine, z);
	delete belowLine, aboveLine;

	if (createdLine) *createdLine = finalLine;
	return true;
}


optional<double> CLinearChart::reverseInterpolateChart(optional<double> xOrYInput, optional<double> zInput) {
	if (!xOrYInput || !zInput) {
		return nullopt;
	}

	TLine* belowLine = nullptr;
	TLine* aboveLine = nullptr;

	double xOrY = *xOrYInput;
	double z = *zInput;

	this->setCalcLines();	
	
	bool foundTheLines = this->getRelevantLines(this->calcLines, z, belowLine, aboveLine);

	if (foundTheLines == false) {
		this->deleteCalcLines();
		return nullopt;//couldnt find a line with a value above or below z value
	}

	if (this->yIsInputAxis) {//Flip points if y is the input axis
		aboveLine->flipPoints();
		if (aboveLine != belowLine) {
			belowLine->flipPoints();//dont want to flip it twice, learned that the hard way
		}
	}

	if (aboveLine->isLinear(false) == false || belowLine->isLinear(false) == false) {
		this->deleteCalcLines();
		return nullopt;//One or both lines can potentially have more then one intersection
	}

	TLine* finalLine = nullptr;
	if (aboveLine == belowLine) {
		finalLine = aboveLine->copyLine();
	}
	else {
		finalLine = new TLine();

		for (auto& point : belowLine->points) {
			finalLine->addNode(new TPoint(0, point->y));
		}
		for (auto& point : aboveLine->points) {
			bool hasThisYAlready = false;
			for (auto& existingPoint : finalLine->points) {
				if (compare_float(existingPoint->y, point->y)) {
					hasThisYAlready = true;
				}
			}
			if (hasThisYAlready == false) {
				finalLine->addNode(new TPoint(0, point->y));
			}
		}
		sort(finalLine->points.begin(), finalLine->points.end(), comparePointsY);

		aboveLine->flipPoints();
		belowLine->flipPoints();


		for (auto& point : finalLine->points) {
			TSegment aboveSegment;
			TSegment belowSegment;
			if (aboveLine->findSegment(aboveSegment, point->y) && belowLine->findSegment(belowSegment, point->y)) {
				double abovePos, belowPos;
				if (aboveSegment.figureYCrossing(abovePos, point->y) && belowSegment.figureYCrossing(belowPos, point->y)) {
					double posDiff = abovePos - belowPos;
					double valDiff = aboveLine->getValue()-belowLine->getValue();
					point->x = (z - belowLine->getValue()) / valDiff * posDiff + belowPos;
				}else {
					delete finalLine;
					this->deleteCalcLines();
					return nullopt;
				}
			}else {
				delete finalLine;
				this->deleteCalcLines();
				return nullopt;
			}

		}
		
	}


	TSegment finalSegment;
	if (finalLine->findSegment(finalSegment, xOrY)) {
		double finalVal;
		if (finalSegment.figureYCrossing(finalVal, xOrY, true)) {
			delete finalLine;
			this->deleteCalcLines();
			return finalVal;
		}
	}
	

	if (finalLine) {
		delete finalLine;
		finalLine = nullptr;
	}
	this->deleteCalcLines();
	return nullopt;
}

optional<double> CLinearChart::figureChart(optional<double> xOrYInput, optional<double> zInput) {
	if (this->reverseInterpolate) {
		return this->reverseInterpolateChart(xOrYInput, zInput);
	}

	if (!xOrYInput || !zInput) { 
		return nullopt; 
	}

	TLine* belowLine = nullptr;
	TLine* aboveLine = nullptr;

	double xOrY = *xOrYInput;
	double z = *zInput;

	this->setCalcLines();

	bool foundTheLines = this->getRelevantLines(this->calcLines, z, belowLine, aboveLine);

	if (foundTheLines==false) {
		this->deleteCalcLines();
		return nullopt;//couldnt find a line with a value above or below z value
	}

	if (this->yIsInputAxis) {//Flip points if y is the input axis
		aboveLine->flipPoints();
		if (aboveLine != belowLine) {
			belowLine->flipPoints();//dont want to flip it twice, learned that the hard way
		}
	}

	if (aboveLine->isLinear(false) == false || belowLine->isLinear(false) == false) {
		this->deleteCalcLines();
		return nullopt;//One or both lines can potentially have more then one intersection
	}


	TSegment belowSegment;
	TSegment aboveSegment;
	if (belowLine->findSegment(belowSegment, xOrY) == false || aboveLine->findSegment(aboveSegment, xOrY) == false) {
		this->deleteCalcLines();
		return nullopt;//couldnt find relevant segments (should be impossible since we are allowing extrapolation)
	}

	double aboveCrossing;
	double belowCrossing;
	if (aboveSegment.figureYCrossing(aboveCrossing, xOrY) == false || belowSegment.figureYCrossing(belowCrossing, xOrY) == false) {
		this->deleteCalcLines();
		return nullopt;//could not determine intersections (possible parallel lines? should be impossible at this point)
	}

	optional<double>returnVal = nullopt;
	if (aboveLine == belowLine) {
		returnVal = aboveCrossing;
	} else {
		returnVal = ((z - belowLine->getValue()) / (aboveLine->getValue() - belowLine->getValue()))*(aboveCrossing - belowCrossing) + belowCrossing;
	}
	this->deleteCalcLines();
	return returnVal;
}

void CLinearChart::calc(ChartProject* chartProject) {
	if (this->inputVariable1==nullptr || this->inputVariable2==nullptr) {
		this->result = nullopt;
		return;
	}

	this->result = this->figureChart(this->inputVariable1->result, this->inputVariable2->result);
}

string CLinearChart::toString() {
	string retString = "linear('" + encodeString(this->getName()) + "'," + to_string(this->exportResult) + "," + to_string(this->yIsInputAxis) + ",";
	retString += this->bounds.toString() + ",";
	string inputVar1 = "''", inputVar2 = "''";
	if (this->inputVariable1) {
		inputVar1 = "'" + encodeString(this->inputVariable1->getName()) + "'";
	}
	if (this->inputVariable2) {
		inputVar2 = "'" + encodeString(this->inputVariable2->getName()) + "'";
	}
	retString += inputVar1 + "," + inputVar2 + ",";
	for (auto & line : this->lines) {
		retString += line->toString();
	}
	retString += "," + to_string(this->reverseInterpolate);
	retString += ")";
	return retString;
}
