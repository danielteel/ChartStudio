#include "stdafx.h"
#include "CInput.h"
#include "helpers.h"
#include "InterpreterCPP.h"



CInput::CInput(string name, string initialValue, string code) : CChartObject(name, ChartObjectType::Input, false) {
	this->setCode(code);
	this->input = initialValue;
	this->invalid = false;
}


CInput::~CInput() {
}

void CInput::setCode(string code) {
	this->code = code;
}

string CInput::getCode() {
	return this->code;
}

bool CInput::isInvalid() {
	return this->invalid;
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
	string retString = "input('" + encodeString(this->getName()) + "','" + encodeString(this->input) + "',";
	retString += "'" + encodeString(this->code) + "')";
	return retString;
}

void CInput::calc(ChartProject* chartProject) {
	InterpreterCPP interpreter;
	interpreter.runCode(chartProject, *this, nullptr, nullptr);
}