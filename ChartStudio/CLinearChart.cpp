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

optional<double> CLinearChart::figureChart(optional<double> xOrYInput, optional<double> zInput) {
	if (!xOrYInput || !zInput) { 
		return nullopt; 
	}

	TLine* belowLine = nullptr;
	TLine* aboveLine = nullptr;

	double xOrY = *xOrYInput;
	double z = *zInput;

	this->setCalcLines();

	for (auto & currentLine : this->calcLines) {
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
			} else {
				belowLine = currentLine;
			}
		}
		if (currentLine->getValue() > z) {
			if (aboveLine != nullptr) {
				if (aboveLine->getValue() > currentLine->getValue()) {
					aboveLine = currentLine;
				}
			} else {
				aboveLine = currentLine;
			}
		}
	}

	if (belowLine == nullptr || aboveLine == nullptr) {
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
	retString += ")";
	return retString;
}
