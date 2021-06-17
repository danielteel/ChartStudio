#include "stdafx.h"
#include "CInput.h"
#include "helpers.h"
#include "InterpreterCPP.h"



CInput::CInput(string name, optional<double> initialValue, string code) : CChartObject(name, ChartObjectType::Input, false) {
	this->setCode(code);
	this->result = initialValue;
}


CInput::~CInput() {
}

void CInput::setCode(string code) {
	this->code = code;
}

string CInput::getCode() {
	return this->code;
}


optional<string> CInput::checkCompile(ChartProject* chartProject) {
	InterpreterCPP interpreter;
	return interpreter.checkCompile(chartProject, this, this->code.c_str(), true);
}

bool CInput::checkIsBad(ChartProject * chartProject, string& whyItsBad) {
	int errorLine = 0;
	optional<string> errorMessage = this->checkCompile(chartProject);
	if (errorMessage != nullopt) {
		whyItsBad = *errorMessage;
		return true;
	}
	whyItsBad = "Good";
	return false;
}

string CInput::toString() {
	string retString = "input('" + encodeString(this->getName()) + "'," + to_string(this->result.value_or(0.0)) + ",";
	retString += "'" + encodeString(this->code) + "')";
	return retString;
}

void CInput::calc(ChartProject* chartProject) {
	InterpreterCPP interpreter;
	interpreter.runCode(chartProject, this, this->code.c_str(), nullptr, nullptr, &this->result);
}
