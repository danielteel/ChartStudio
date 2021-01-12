#pragma once
#include <string>
#include <vector>
#include "CChartBase.h"
using namespace std;

class CTrendChart : public CChartBase {
public:
	CTrendChart(string name, bool exportResult, bool yIsInputAxis, BoundSettings bounds, vector<TLine*> lines);
	CTrendChart(string name, bool exportResult, bool yIsInputAxis, BoundSettings bounds, string in1Link, string in2Link, string in3Link, vector<TLine*> lines);
	~CTrendChart();

	bool checkIsBad(ChartProject* chartProject, string& whyItsBad) override;

	string entrpointInputLink;
	CChartObject* entryPointInput = nullptr;

	optional<double> figureTrend(optional<double> xOryInput, optional<double> entryPointInput, optional<double> exitPointInput);
	void calc(ChartProject* chartProject) override;

	string toString() override;
};
