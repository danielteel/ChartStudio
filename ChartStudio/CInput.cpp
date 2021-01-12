#include "stdafx.h"
#include "CInput.h"
#include "helpers.h"
#include "Interpreter.h"



CInput::CInput(string name, optional<double> initialValue, string code) : CChartObject(name, ChartObjectType::Input, false) {
	this->setCode(code);
	this->result = initialValue;
	this->interpreter = new Interpreter();
}


CInput::~CInput() {
	delete this->interpreter;
}

void CInput::setCode(string code) {
	this->code = code;
}

string CInput::getCode() {
	return this->code;
}

bool CInput::checkCompile(ChartProject* chartProject, int & errorLine, string & errorMessage) {
	return this->interpreter->checkCompile(chartProject, this, this->code, errorLine, errorMessage);
}

bool CInput::checkIsBad(ChartProject * chartProject, string& whyItsBad) {
	int errorLine = 0;
	string errorMessage;
	if (!this->checkCompile(chartProject, errorLine, errorMessage)) {
		whyItsBad = "Error on line " + to_string(errorLine) + ": " + errorMessage;
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
	this->interpreter->runCode(chartProject, this, this->code, this->result);
}
