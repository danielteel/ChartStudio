#include "stdafx.h"
#include "CConstant.h"
#include "helpers.h"

CConstant::CConstant(string name, double value) : CChartObject(name, ChartObjectType::Constant, false) {
	this->result = value;
}


CConstant::~CConstant() {
}

string CConstant::toString() {
	return "constant('" + encodeString(this->getName()) + "'," + trimmedDoubleToString(this->result.value_or(0.0)) + ")";
}

void CConstant::calc(ChartProject* chartProject) {
}
