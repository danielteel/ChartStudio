#pragma once
#include <string>
#include <vector>
#include <optional>
#include "ExternalDef.h"
using namespace std;

class ChartProject;
class CChartObject;
class CInput;
class CScript;

class InterpreterCPP {
public:
	vector<ExternalDef> buildExternList(ChartProject * chartProject, CChartObject * thisChartObject);

	void cleanUpExternList(vector<ExternalDef>& externals);

	optional<string> checkCompile(ChartProject* chartProject, CChartObject* thisChartObject, string code, bool isInput);
	void runCode(ChartProject * chartProject, CScript& thisChartObject, bool * errorOccured, string * errorOut);
	void runCode(ChartProject* chartProject, CInput& thisChartObject, bool* errorOccured, string* errorOut);
};
