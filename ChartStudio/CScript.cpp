#include "stdafx.h"
#include "CScript.h"
#include "helpers.h"
#include "InterpreterCPP.h"


CScript::CScript(string name, bool exportResult, string code) : CChartObject(name, ChartObjectType::Script, exportResult) {
	this->setCode(code);
}


CScript::~CScript() {
}

void CScript::setCode(string code) {
	this->code = code;
}

string CScript::getCode() {
	return this->code;
}

optional<string> CScript::checkCompile(ChartProject* chartProject) {
	InterpreterCPP interpreter;
	return interpreter.checkCompile(chartProject, this, this->code.c_str(), false);
}

bool CScript::checkIsBad(ChartProject * chartProject, string& whyItsBad) {
	int errorLine = 0;
	optional<string> errorMessage= this->checkCompile(chartProject);
	if (errorMessage!=nullopt) {
		whyItsBad = *errorMessage;
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
	InterpreterCPP interpreter;

	this->result=interpreter.runCode(chartProject, this, this->code, nullptr, nullptr);
}

