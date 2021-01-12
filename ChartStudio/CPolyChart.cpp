#include "stdafx.h"
#include "CPolyChart.h"
#include "TLine.h"
#include "TPoint.h"
#include "TSegment.h"
#include "helpers.h"

CPolyChart::CPolyChart(string name, bool exportResult, BoundSettings bounds, vector<TLine*> lines):CChartBase(name, ChartObjectType::PolyChart, exportResult, false, bounds, lines){
}

CPolyChart::CPolyChart(string name, bool exportResult, BoundSettings bounds, string in1Link, string in2Link, vector<TLine*> lines) : CChartBase(name, ChartObjectType::PolyChart, exportResult, false, bounds, in1Link, in2Link, lines) {
}

CPolyChart::~CPolyChart() {
}

bool CPolyChart::checkIsBad(ChartProject* chartProject, string& whyItsBad) {
	bool isBad = false;
	whyItsBad = "";
	if (this->inputVariable1 == nullptr || this->inputVariable2 == nullptr) {
		whyItsBad += "Empty input(s);\r\n";
		isBad = true;
	}

	if (this->lines.size() == 0) {
		whyItsBad += "No polygons;\r\n";
		isBad = true;
	}
	for (auto & line : this->lines) {
		if (line->points.size() < 3) {
			whyItsBad += "Incomplete polygons;\r\n";
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

void CPolyChart::calc(ChartProject* chartProject) {
	if (this->inputVariable1 == nullptr || this->inputVariable2 == nullptr) {
		this->result = nullopt;
		return;
	}

	this->result = this->hitTestPoly(this->inputVariable1->result, this->inputVariable2->result);
}

optional<double> CPolyChart::hitTestPoly(optional<double> xIn, optional<double> yIn) {
	if (!xIn || !yIn) {
		return nullopt;
	}

	this->setCalcLines();

	double minX = 0;
	double maxX = 0;
	for (auto & currentLine : this->calcLines) {
		for (auto & currentPoint : currentLine->points) {
			if (currentPoint->x > maxX) {
				maxX = currentPoint->x;
			}
			if (currentPoint->x < minX) {
				minX = currentPoint->x;
			}
		}
	}

	TSegment horizantalSegment(TPoint(minX-1.0f, *yIn), TPoint(maxX+1.0f, *yIn));

	for (auto & currentLine : this->calcLines) {
		int intersectsLeftOfX = 0;
		int numOfPoints = currentLine->points.size();

		if (numOfPoints > 2) {

			TSegment currentSegment(0, 0);
			TPoint intersect;

			for (int i = 0; i < numOfPoints; i++) {

				currentSegment.point1 = *currentLine->points[i];
				if (i == numOfPoints - 1) {
					currentSegment.point2 = *currentLine->points[0];
				} else {
					currentSegment.point2 = *currentLine->points[i+1];
				}

				if (currentSegment.getSegmentIntersection(intersect,horizantalSegment, true)) {
					if (intersect.x <= *xIn) {
						intersectsLeftOfX++;
					}
				}

			}

			if (intersectsLeftOfX > 0 && intersectsLeftOfX % 2 > 0) {
				double retVal = currentLine->getValue();
				this->deleteCalcLines();
				return retVal;
			}

		}
	}

	//no hit test, we failed
	this->deleteCalcLines();
	return nullopt;
}


string CPolyChart::toString() {
	string retString = "poly('" + encodeString(this->getName()) + "'," + to_string(this->exportResult) + ",";
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
