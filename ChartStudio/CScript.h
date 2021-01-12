#pragma once
#include "CChartObject.h"
#include <string>


using namespace std;

class Interpreter;
class ChartProject;

class CScript :
	public CChartObject {
public:
	CScript(string name, bool exportResult, string code);
	~CScript();

	void setCode(string code);
	string getCode();

	bool checkCompile(ChartProject* chartProject, int& errorLine, string& errorMessage);

	bool checkIsBad(ChartProject* chartProject, string& whyItsBad) override;

	string toString() override;

	string compileToJavascript();

	void calc(ChartProject* chartProject) override;

private:
	string code;
	Interpreter* interpreter;
};

