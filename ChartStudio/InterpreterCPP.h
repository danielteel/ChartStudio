#pragma once
#include <string>
#include <vector>
#include <optional>
#include "ExternalDef.h"
using namespace std;

class ChartProject;
class CChartObject;


class InterpreterCPP {
public:
	vector<ExternalDef> buildExternList(ChartProject * chartProject, CChartObject * thisChartObject);

	void cleanUpExternList(vector<ExternalDef>& externals);

	optional<string> checkCompile(ChartProject* chartProject, CChartObject* thisChartObject, string code, bool hasThis);
	optional<double> runCode(ChartProject* chartProject, CChartObject* thisChartObject, string code, bool* errorOccured, string* errorOut, optional<double>* thisValue=nullptr);
};
