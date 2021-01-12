#include "stdafx.h"
#include "CTrendChart.h"
#include "TLine.h"
#include "TPoint.h"
#include "TSegment.h"
#include "helpers.h"

CTrendChart::CTrendChart(string name, bool exportResult, bool yIsInputAxis, BoundSettings bounds, vector<TLine*> lines) : CChartBase(name, ChartObjectType::TrendChart, exportResult, yIsInputAxis, bounds, lines) {
}
CTrendChart::CTrendChart(string name, bool exportResult, bool yIsInputAxis, BoundSettings bounds, string in1Link, string in2Link, string in3Link, vector<TLine*> lines) : CChartBase(name, ChartObjectType::TrendChart, exportResult, yIsInputAxis, bounds, in1Link, in2Link, lines) {
	this->entrpointInputLink = in3Link;
}

CTrendChart::~CTrendChart() {
}

bool CTrendChart::checkIsBad(ChartProject* chartProject, string& whyItsBad) {
	bool isBad = false;
	whyItsBad = "";
	if (this->inputVariable1 == nullptr || this->inputVariable2 == nullptr || this->entryPointInput == nullptr) {
		whyItsBad += "Empty input(s);\r\n";
		isBad = true;
	}

	if (this->lines.size() < 2) {
		whyItsBad += "Not enough lines;\r\n";
		isBad = true;
	}
	for (auto & line : this->lines) {
		if (line->points.size() < 2) {
			whyItsBad += "Line(s) has less then 2 points;\r\n";
			isBad = true;
			break;
		}
		if (line->isLinear(!this->yIsInputAxis) == false) {
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

optional<double> CTrendChart::figureTrend(optional<double> xOryInput, optional<double> entryPointInput, optional<double> exitPointInput) {
	if (!xOryInput || !entryPointInput || !exitPointInput) {
		return nullopt;
	}

	this->setCalcLines();

	double xOrY = *xOryInput;
	double entryPoint = *entryPointInput;
	double exitPoint = *exitPointInput;

	TLine* leftLineFind = nullptr;
	TLine* rightLineFind = nullptr;
	double leftEntryCross = 0.0f;
	double rightEntryCross = 0.0f;

	for (auto & currentLine : this->calcLines) {
		if (!this->yIsInputAxis) {
			currentLine->flipPoints();
		}

		if (currentLine->isLinear(false) == false) {
			this->deleteCalcLines();
			return nullopt;//A line is not 'linear' and could result in improper calculations, fix the chart.
		}
		TSegment currentSegment;
		if (currentLine->findSegment(currentSegment, entryPoint)) {
			double yCrossing;
			if (currentSegment.figureYCrossing(yCrossing, entryPoint)) {
				if (yCrossing >= xOrY) {
					if (rightLineFind == nullptr) {
						rightLineFind = currentLine;
						rightEntryCross = yCrossing;
					} else {
						if (yCrossing < rightEntryCross) {
							rightLineFind = currentLine;
							rightEntryCross = yCrossing;
						}
					}
				}
				if (yCrossing <= xOrY) {
					if (leftLineFind == nullptr) {
						leftLineFind = currentLine;
						leftEntryCross = yCrossing;
					} else {
						if (yCrossing > leftEntryCross) {
							leftLineFind = currentLine;
							leftEntryCross = yCrossing;
						}
					}
				}
			}
		}
	}

	if (leftLineFind == nullptr || rightLineFind == nullptr) {
		this->deleteCalcLines();
		return nullopt;//Could not find either the left or right relevant lines, couldnt calculate the entry point
	}

	TSegment leftSegment;
	TSegment rightSegment;
	if (leftLineFind->findSegment(leftSegment, exitPoint) == false || rightLineFind->findSegment(rightSegment, exitPoint) == false) {
		this->deleteCalcLines();
		return nullopt;//could not calculate exit point
	}

	double leftEndCross;
	double rightEndCross;
	if (leftSegment.figureYCrossing(leftEndCross, exitPoint) == false || rightSegment.figureYCrossing(rightEndCross, exitPoint) == false) {
		this->deleteCalcLines();
		return nullopt;//could not calculate exit point intersections
	}

	this->deleteCalcLines();
	double entryRatio = (xOrY - leftEntryCross) / (rightEntryCross - leftEntryCross);
	return (rightEndCross - leftEndCross) * entryRatio + leftEndCross;
}

void CTrendChart::calc(ChartProject * chartProject) {
	if (this->inputVariable1 == nullptr || this->inputVariable2 == nullptr || this->entryPointInput == nullptr) {
		this->result = nullopt;
		return;
	}

	this->result = this->figureTrend(this->inputVariable1->result, this->entryPointInput->result, this->inputVariable2->result);
}

string CTrendChart::toString() {
	string retString = "trend('" + encodeString(this->getName()) + "'," + to_string(this->exportResult) + "," + to_string(this->yIsInputAxis) + ",";
	retString += this->bounds.toString()+",";
	string inputVar1="''", inputVar2="''", inputVar3="''";
	if (this->inputVariable1) {
		inputVar1 = "'" + encodeString(this->inputVariable1->getName()) + "'";
	}
	if (this->inputVariable2) {
		inputVar2 = "'" + encodeString(this->inputVariable2->getName()) + "'";
	}
	if (this->entryPointInput) {
		inputVar3 = "'" + encodeString(this->entryPointInput->getName()) + "'";
	}
	retString += inputVar1 + "," + inputVar2 + "," + inputVar3 + ",";
	for (auto & line : this->lines) {
		retString += line->toString();
	}
	retString += ")";
	return retString;
}
