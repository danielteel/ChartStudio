#pragma once
#include "CChartObject.h"
#include <string>
using namespace std;

class Interpreter;

class CInput :
	public CChartObject {
public:
	CInput(string name, optional<double> intialValue, string code="");
	virtual ~CInput();


	void setCode(string code);
	string getCode();

	bool checkCompile(ChartProject* chartProject, int& errorLine, string& errorMessage);

	bool checkIsBad(ChartProject* chartProject, string& whyItsBad) override;

	string toString() override;

	void calc(ChartProject* chartProject) override;

private:
	string code;
	Interpreter* interpreter;
};

