#pragma once
#include <vector>
#include <string>
#include "CChartBase.h"
using namespace std;

class TLine;

class CLinearChart : public CChartBase {
public:
	CLinearChart(string name, bool exportResult, bool yIsInputAxis, BoundSettings bounds, vector<TLine*> lines);
	CLinearChart(string name, bool exportResult, bool yIsInputAxis, BoundSettings bounds, string in1Link, string in2Link, vector<TLine*> lines);
	~CLinearChart();

	bool checkIsBad(ChartProject* chartProject, string& whyItsBad) override;

	optional<double> figureChart(optional<double> xOrY, optional<double> z);
	void calc(ChartProject* chartProject) override;

	string toString() override;
};