#pragma once
#include <vector>
#include <string>
#include "CChartObject.h"

using namespace std;

class CTableEntry {
public:
	friend class CTable;

	string chartObjectLink;

	CChartObject* chartObject = nullptr;

	CTableEntry(CChartObject* reference, double value) {
		this->chartObject = reference;
		this->value = value;
	}

	CTableEntry(string chartObjectLink, double value) {
		this->chartObjectLink = chartObjectLink;
		this->value = value;
	}

	double getValue() {
		return this->value;
	}

	string toString() {
		if (chartObject) {
			return "('"+chartObject->getName()+"', " + trimmedDoubleToString(this->value) + ")";
		}
		return "('', " + trimmedDoubleToString(this->value) + ")";
	}

private:
	double value = 0;

};

class CTable : public CChartObject {
public:
	CTable(string name, bool exportResult);
	~CTable();

	vector<CTableEntry*> table;

	string inputVariableLink;
	CChartObject* inputVariable = nullptr;
	
	void addTableEntry(CTableEntry* newEntry);
	void deleteTableEntry(CTableEntry* newEntry);
	void sortItems();
	bool checkIsBad(ChartProject * chartProject, string & whyItsBad) override;

	void setItemValue(CTableEntry* entry, double value);

	string toString() override;

	optional<double> figureTable(optional<double> value);
	void calc(ChartProject* chartProject) override;
};

