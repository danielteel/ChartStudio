#pragma once
#include "CChartBase.h"
#include <string>
using namespace std;

class TLine;

class CClampChart :
	public CChartBase {
public:
	CClampChart(string name, bool exportResult, bool yIsInputAxis, BoundSettings bounds, TLine * minLine, TLine * maxLine);
	CClampChart(string name, bool exportResult, bool yIsInputAxis, BoundSettings bounds, string input1Link, string input2Link, TLine * minLine, TLine * maxLine);
	~CClampChart();

	void setCalcLines() override;
	void deleteCalcLines() override;

	optional<double> figureClamp(optional<double> x, optional<double> valueToClamp);
	void calc(ChartProject* chartProject) override;

	void deleteLine(TLine* line) override;
	TLine* addLine(double value) override;
	void addLine(TLine * line) override;
	TLine* minLine = nullptr;
	TLine* maxLine = nullptr;

	TLine* calcMinLine = nullptr;
	TLine* calcMaxLine = nullptr;

	bool checkIsBad(ChartProject* chartProject, string& whyItsBad) override;

	string toString() override;


private:
	void updateClampLines();


};

