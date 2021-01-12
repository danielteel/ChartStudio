#pragma once
#include <string>
#include <optional>

using namespace std;

class ChartProject;
class CChartBase;
class CClampChart;
class CLinearChart;
class CPolyChart;
class CTrendChart;
class CScript;
class CTable;
class CConstant;
class CInput;

enum class ChartObjectType {
	Abstract,
	ClampChart,
	LinearChart,
	TrendChart,
	PolyChart,
	Constant,
	Input,
	Script,
	Table,
};

class CChartObject {
public:
	CChartObject(string name, ChartObjectType type, bool exportResult);
	virtual ~CChartObject() = 0;

	CChartBase * toChartBase();
	CTrendChart * toTrendChart();
	CClampChart * toClampChart();
	CPolyChart * toPolyChart();
	CLinearChart * toLinearChart();
	CTable * toTable();
	CScript * toScript();
	CInput * toInput();
	CConstant * toConstant();

	ChartObjectType type = ChartObjectType::Abstract;

	string getName();
	static string getFormattedName(string desiredName);
	void setName(string);

	optional<double> result = nullopt;
	bool exportResult = true;

	virtual bool checkIsBad(ChartProject* chartProject, string& whyItsBad);

	virtual string toString() = 0;
	virtual void calc(ChartProject* chartProject) = 0;

	unsigned int tempIndex;

private: 
	string name;

};