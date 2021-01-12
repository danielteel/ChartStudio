#include "stdafx.h"
#include "TLine.h"
#include "TSegment.h"
#include "CClampChart.h"
#include "helpers.h"

CClampChart::CClampChart(string name, bool exportResult, bool yIsInputAxis, BoundSettings bounds, TLine* minLine, TLine* maxLine) : CChartBase(name, ChartObjectType::ClampChart, exportResult, yIsInputAxis, bounds, {}) {
	this->minLine = minLine;
	this->maxLine = maxLine;
	this->updateClampLines();
}

CClampChart::CClampChart(string name, bool exportResult, bool yIsInputAxis, BoundSettings bounds, string input1Link, string input2Link, TLine* minLine, TLine* maxLine) : CChartBase(name, ChartObjectType::ClampChart, exportResult, yIsInputAxis, bounds, input1Link, input2Link, {}) {
	this->minLine = minLine;
	this->maxLine = maxLine;
	this->updateClampLines();
}


CClampChart::~CClampChart() {
	this->updateClampLines();//to free any extra lines
	this->lines.clear();

	if (this->minLine) {
		delete this->minLine;
	}
	if (this->maxLine) {
		delete this->maxLine;
	}
}

void CClampChart::setCalcLines() {
	deleteCalcLines();

	if (this->minLine) {
		this->calcMinLine = this->minLine->copyLine();
		if (this->calcMinLine) this->calcLines.push_back(this->calcMinLine);
	}
	if (this->maxLine) {
		this->calcMaxLine = this->maxLine->copyLine();
		if (this->calcMaxLine) this->calcLines.push_back(this->calcMaxLine);
	}

	for (auto& line : this->calcLines) {
		for (auto & point : line->points) {
			this->screenToChart(point);
		}
	}
}

void CClampChart::deleteCalcLines() {
	this->calcMinLine = nullptr;
	this->calcMaxLine = nullptr;

	if (this->calcLines.empty()) return;
	for (auto & calcLine : this->calcLines) {
		delete calcLine;
	}
	this->calcLines.clear();
}



bool CClampChart::checkIsBad(ChartProject* chartProject, string& whyItsBad) {
	bool isBad = false;
	whyItsBad = "";
	if (this->inputVariable1 == nullptr || this->inputVariable2 == nullptr) {
		whyItsBad += "Empty input(s);\r\n";
		isBad = true;
	}

	if (this->maxLine->points.size() == 0 && this->minLine->points.size() == 0) {
		whyItsBad += "No clamps lines;\r\n";
		isBad = true;
	}

	if (this->maxLine->points.size() > 0 && this->maxLine->points.size() < 2) {
		whyItsBad += "Max line has less than 2 nodes;\r\n";
		isBad = true;
	} else if (this->maxLine->points.size() > 0 && this->maxLine->isLinear(this->yIsInputAxis) == false) {
		whyItsBad += "Max line is not linear;\r\n";
		isBad = true;
	}

	if (this->minLine->points.size() > 0 && this->minLine->points.size() < 2) {
		whyItsBad += "Min line has less than 2 nodes;\r\n";
		isBad = true;
	} else if (this->minLine->points.size() > 0 && this->minLine->isLinear(this->yIsInputAxis) == false) {
		whyItsBad += "Min line is not linear;\r\n";
		isBad = true;
	}
	if (isBad == false) {
		whyItsBad = "Good";
	} else {
		whyItsBad = whyItsBad.substr(0, whyItsBad.length() - 2);//trim last \r\n
	}
	return isBad;
}

void CClampChart::updateClampLines() {
	//empty the line vector
	this->lines.clear();

	//make empty lines if any of the min or max lines are nullptr
	if (this->minLine == nullptr) {
		this->minLine = new TLine();
	}
	if (this->maxLine == nullptr) {
		this->maxLine = new TLine();
	}

	//add min/max lines to lines vector
	this->lines.push_back(this->minLine);
	this->lines.push_back(this->maxLine);
}

optional<double> CClampChart::figureClamp(optional<double> xInput, optional<double> valueToClampInput) {
	if (!xInput || !valueToClampInput) {
		return nullopt;
	}
	this->setCalcLines();
	if (this->yIsInputAxis) {
		if (this->calcMinLine) this->calcMinLine->flipPoints();
		if (this->calcMaxLine) this->calcMaxLine->flipPoints();
	}
	double x = *xInput;
	double valueToClamp = *valueToClampInput;

	if (this->calcMinLine && this->calcMinLine->points.empty() == false) {
		if (this->calcMinLine->isLinear(false) == false) {
			this->deleteCalcLines();
			return nullopt;//failed
		} else {
			TSegment minSegment;
			if (this->calcMinLine->findSegment(minSegment, x) == false) {
				this->deleteCalcLines();
				return nullopt;//failed
			} else {
				double minValue;
				if (minSegment.figureYCrossing(minValue, x) == false) {
					this->deleteCalcLines();
					return nullopt;//failed
				} else {
					//passed
					if (valueToClamp < minValue) {
						valueToClamp = minValue;
					}
				}
			}
		}
	}

	if (this->calcMaxLine && this->calcMaxLine->points.empty() == false) {
		if (this->calcMaxLine->isLinear(false) == false) {
			this->deleteCalcLines();
			return nullopt;//failed
		} else {
			TSegment maxSegment;
			if (this->calcMaxLine->findSegment(maxSegment, x) == false) {
				this->deleteCalcLines();
				return nullopt;//failed
			} else {
				double maxValue;
				if (maxSegment.figureYCrossing(maxValue, x) == false) {
					this->deleteCalcLines();
					return nullopt;//failed
				} else {
					//passed
					if (valueToClamp > maxValue) {
						valueToClamp = maxValue;
					}
				}
			}
		}
	}

	this->deleteCalcLines();
	return valueToClamp;
}

void CClampChart::calc(ChartProject * chartProject) {
	if (this->inputVariable1 == nullptr || this->inputVariable2 == nullptr) {
		this->result = nullopt;
		return;
	}

	this->result = this->figureClamp(this->inputVariable1->result, this->inputVariable2->result);
}


void CClampChart::deleteLine(TLine * line) {
	if (this->minLine == line) {
		delete this->minLine;
		this->minLine = new TLine();
	}
	if (this->maxLine == line) {
		delete this->maxLine;
		this->maxLine = new TLine();
	}
	updateClampLines();
}

TLine* CClampChart::addLine(double value) {
	//we're not going to actually add a line, we're just going to return the next empty line
	if (this->minLine && this->minLine->points.empty()) return this->minLine;
	if (this->maxLine && this->maxLine->points.empty()) return this->maxLine;
	return nullptr;
}

void CClampChart::addLine(TLine * line) {
	*(char*)(0) = 0;//crash on purpose as you should never try to add a line to clamp chart like this
}

string CClampChart::toString() {
	string retString = "clamp('" + encodeString(this->getName()) + "'," + to_string(this->exportResult) + "," + to_string(this->yIsInputAxis) + ",";
	retString += this->bounds.toString() + ",";
	string inputVar1 = "''", inputVar2 = "''";
	if (this->inputVariable1) {
		inputVar1 = "'" + encodeString(this->inputVariable1->getName()) + "'";
	}
	if (this->inputVariable2) {
		inputVar2 = "'" + encodeString(this->inputVariable2->getName()) + "'";
	}
	retString += inputVar1 + "," + inputVar2 + ",";
	retString += this->maxLine->toString();
	retString += this->minLine->toString();
	retString += ")";
	return retString;
}
