#pragma once
#include "CChartObject.h"
#include <string>
using namespace std;

class CConstant :
	public CChartObject {
public:
	CConstant(string name, double value);
	virtual ~CConstant();

	string toString() override;

	void calc(ChartProject* chartProject) override;
};

