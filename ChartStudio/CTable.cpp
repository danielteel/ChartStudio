#pragma once
#include "stdafx.h"
#include "helpers.h"
#include "CTable.h"

#include "ChartProject.h"


CTable::CTable(string name, bool exportResult) : CChartObject(name, ChartObjectType::Table, exportResult) {
}


CTable::~CTable() {
	for (auto & entry : this->table) {
		delete entry;
	}
}

void CTable::addTableEntry(CTableEntry * newEntry) {
	this->table.push_back(newEntry);
	this->sortItems();
}

void CTable::deleteTableEntry(CTableEntry * deleteThis) {
	for (auto it = this->table.begin(); it != this->table.end(); it++) {
		if (*it == deleteThis) {
			delete *it;
			it = this->table.erase(it);
			break;
		}
	}
}

void CTable::sortItems() {
	if (this->table.size() < 2) return;

	bool notSwapped;
	do {
		notSwapped = true;
		for (size_t i = 0; i < this->table.size() - 1; i++) {
			if (this->table[i]->value > this->table[i + 1]->value) {
				CTableEntry* swapper = this->table[i];
				this->table[i] = this->table[i + 1];
				this->table[i + 1] = swapper;
				notSwapped = false;
			}
		}
	} while (notSwapped == false);
}

bool CTable::checkIsBad(ChartProject * chartProject, string& whyItsBad) {
	bool inputRefBad = false;
	bool duplicatesExist = false;
	bool isBad = false;
 
	whyItsBad = "";

	if (this->inputVariable == nullptr) {
		whyItsBad += "Empty input;\r\n";
		isBad = true;
	}

	if (this->table.size() < 2) {
		whyItsBad += "Less than 2 entries;\r\n";
		isBad = true;
	}

	for (auto & entry : this->table) {
		if (inputRefBad==false && entry->chartObject == nullptr) {
			whyItsBad += "Input reference(s) null;\r\n";
			inputRefBad = true;
		}

		if (duplicatesExist == false) {
			for (auto & checkAgainst : this->table) {
				if (entry != checkAgainst) {
					if (compare_float(entry->value,checkAgainst->value)==true) {
						whyItsBad += "Duplicate item values;\r\n";
						duplicatesExist = true;
						break;
					}
				}
			}
		}
	}

	return inputRefBad || duplicatesExist || isBad;
}

void CTable::setItemValue(CTableEntry * entry, double value) {
	for (auto & item : this->table) {
		if (entry == item) {
			entry->value = value;
			this->sortItems();
			break;
		}
	}
}

string CTable::toString() {
	string retString = "table('"+encodeString(this->getName())+"',"+to_string(this->exportResult)+",";

	if (this->inputVariable) {
		retString += "'" + encodeString(this->inputVariable->getName()) + "',";
	} else {
		retString += "'',";
	}

	for (auto & item : this->table) {
		retString += item->toString();
	}
	retString += ")";
	return retString;
}


optional<double> CTable::figureTable(optional<double> value) {
	if (!value) {
		return nullopt;
	}

	CTableEntry* below = nullptr;
	CTableEntry* above = nullptr;

	for (auto & item : this->table) {
		if (compare_float(item->value, *value)) {
			below = item;
			above = item;
			break;
		}
		if (item->value <= *value) {
			if (below == nullptr) {
				below = item;
			} else if (item->value > below->value) {
				below = item;
			}
		}
		if (item->value >= *value) {
			if (above == nullptr) {
				above = item;
			} else if (item->value < above->value) {
				above = item;
			}
		}
	}

	if (above == nullptr || below == nullptr) {
		return nullopt;
	}
	if (above->chartObject == nullptr || below->chartObject == nullptr) {
		return nullopt;
	}
	if (!above->chartObject->result || !below->chartObject->result) {
		return nullopt;
	}

	if (above == below || compare_float(*above->chartObject->result, *below->chartObject->result)) {
		return above->chartObject->result;
	}
	
	double resultDiff = *above->chartObject->result - *below->chartObject->result;
	double valDiff = above->value - below->value;

	return ((*value - below->value) / valDiff) * resultDiff + *below->chartObject->result;
}

void CTable::calc(ChartProject * chartProject) {
	if (this->inputVariable == nullptr) {
		this->result = nullopt;
		return;
	}

	this->result = this->figureTable(this->inputVariable->result);
}
