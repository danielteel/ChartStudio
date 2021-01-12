#include "stdafx.h"
#include "ChartProject.h"
#include "CChartObject.h"
#include "helpers.h"

CChartObject::CChartObject(string name, ChartObjectType type, bool exportResult) {
	this->setName(name);
	this->type = type;
	this->result = nullopt;
	this->exportResult = exportResult;
}



CChartObject::~CChartObject() {
}


CChartBase* CChartObject::toChartBase() {
	switch (this->type) {
	case ChartObjectType::ClampChart:
	case ChartObjectType::LinearChart:
	case ChartObjectType::PolyChart:
	case ChartObjectType::TrendChart:
		return reinterpret_cast<CChartBase*>(this);
	}
	return nullptr;
}

CTrendChart* CChartObject::toTrendChart() {
	if (this->type == ChartObjectType::TrendChart) {
		return reinterpret_cast<CTrendChart*>(this);
	}
	return nullptr;
}

CClampChart* CChartObject::toClampChart() {
	if (this->type == ChartObjectType::ClampChart) {
			return reinterpret_cast<CClampChart*>(this);
	}
	return nullptr;
}

CPolyChart* CChartObject::toPolyChart() {
	if (this->type == ChartObjectType::PolyChart) {
		return reinterpret_cast<CPolyChart*>(this);
	}
	return nullptr;
}

CLinearChart* CChartObject::toLinearChart() {
	if (this->type == ChartObjectType::LinearChart) {
		return reinterpret_cast<CLinearChart*>(this);
	}
	return nullptr;
}

CTable* CChartObject::toTable() {
	if (this->type == ChartObjectType::Table) {
		return reinterpret_cast<CTable*>(this);
	}
	return nullptr;
}

CScript* CChartObject::toScript() {
	if (this->type == ChartObjectType::Script) {
		return reinterpret_cast<CScript*>(this);
	}
	return nullptr;
}
CInput* CChartObject::toInput() {
	if (this->type == ChartObjectType::Input) {
		return reinterpret_cast<CInput*>(this);
	}
	return nullptr;
}
CConstant* CChartObject::toConstant() {
	if (this->type == ChartObjectType::Constant) {
		return reinterpret_cast<CConstant*>(this);
	}
	return nullptr;
}

string CChartObject::getName() {
	return this->name;
}

string CChartObject::getFormattedName(string desiredName) {
	trim(desiredName);
	if (desiredName.length() < 1) {
		desiredName = "Unnamed";
	}
	if (!(isAlpha(desiredName[0]) || desiredName[0] == '_')) {
		desiredName = "_" + desiredName;
	}
	for (size_t i = 0; i < desiredName.length(); i++) {
		if (!(isAlpha(desiredName[i]) || isDigit(desiredName[i]) || desiredName[i] == '_' || desiredName[i] == '.')) {
			desiredName[i] = '_';
		}
	}
	return desiredName;
}

void CChartObject::setName(string toThis) {
	this->name = CChartObject::getFormattedName(toThis);
}

bool CChartObject::checkIsBad(ChartProject* chartProject, string& whyItsBad) {
	if (this->type == ChartObjectType::Abstract) {
		whyItsBad = "Object is abstract type, should be impossible";
		return true;
	}
	whyItsBad = "Good";
	return false;
}
