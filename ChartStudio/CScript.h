#pragma once
#include "CChartObject.h"
#include <string>

using namespace std;

class ChartProject;

class CScript :
	public CChartObject {
public:
	CScript(string name, bool exportResult, string code);
	~CScript();


	void setCode(string code);
	string getCode();

	optional<string> checkCompile(ChartProject* chartProject);
	bool checkIsBad(ChartProject* chartProject, string& whyItsBad) override;

	string toString() override;

	void calc(ChartProject* chartProject) override;

	string code;
};

