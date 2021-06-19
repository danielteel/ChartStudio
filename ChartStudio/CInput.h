#pragma once
#include "CChartObject.h"
#include <string>

using namespace std;

class ChartProject;

class CInput :
	public CChartObject {
public:
	CInput(string name, optional<double> intialValue, string code="");
	virtual ~CInput();


	void setCode(string code);
	string getCode();

	bool isInvalid();

	optional<string> checkCompile(ChartProject* chartProject);
	bool checkIsBad(ChartProject* chartProject, string& whyItsBad) override;

	string toString() override;

	void calc(ChartProject* chartProject) override;

private:
	string code;
	optional<bool> invalid = false;
};

