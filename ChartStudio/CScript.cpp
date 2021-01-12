#include "stdafx.h"
#include "CScript.h"
#include "helpers.h"
#include "Interpreter.h"

CScript::CScript(string name, bool exportResult, string code) : CChartObject(name, ChartObjectType::Script, exportResult) {
	this->setCode(code);
	this->interpreter = new Interpreter();
}


CScript::~CScript() {
	delete this->interpreter;
}

void CScript::setCode(string code) {
	this->code = code;
}

string CScript::getCode() {
	return this->code;
}

bool CScript::checkCompile(ChartProject* chartProject, int & errorLine, string & errorMessage) {
	return this->interpreter->checkCompile(chartProject, this, this->code, errorLine, errorMessage);
}

bool CScript::checkIsBad(ChartProject * chartProject, string& whyItsBad) {
	int errorLine = 0;
	string errorMessage;
	if (!this->checkCompile(chartProject, errorLine, errorMessage)) {
		whyItsBad = "Error on line " + to_string(errorLine) + ": " + errorMessage;
		return true;
	}
	whyItsBad = "Good";
	return false;
}

string CScript::toString() {
	string retString = "script('" + encodeString(this->getName()) + "'," + to_string(this->exportResult) + ",";
	retString += "'"+encodeString(this->code)+"')";
	return retString;
}

void CScript::calc(ChartProject* chartProject) {
	this->interpreter->runCode(chartProject, this, this->code, this->result);
}

