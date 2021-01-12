#pragma once
#include <string>
#include <vector>
#include "CChartBase.h"
using namespace std;

class TLine;

class CPolyChart : public CChartBase {
public:
	CPolyChart(string name, bool exportResult, BoundSettings bounds, vector<TLine*> lines);
	CPolyChart(string name, bool exportResult, BoundSettings bounds, string in1Link, string in2Link, vector<TLine*> lines);
	virtual ~CPolyChart();

	void calc(ChartProject* chartProject) override;

	optional<double> hitTestPoly(optional<double> x, optional<double> y);

	bool checkIsBad(ChartProject* chartProject, string& whyItsBad) override;

	string toString() override;

};

