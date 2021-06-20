#include "stdafx.h"
#include "InterpreterCPP.h"
#include <string>
#include "Tokenizer.h"
#include "Parser.h"
#include "OpObj.h"
#include "ExternalDef.h"

#include "ChartProject.h"
#include "CChartObject.h"
#include "CChartBase.h"
#include "CLinearChart.h"
#include "CClampChart.h"
#include "CPolyChart.h"
#include "CScript.h"
#include "CInput.h"
#include "CConstant.h"
#include "CTrendChart.h"
#include "ChartProject.h"


ChartProject* currentProject = nullptr;


optional<CChartObject*> findChart(string name, ChartObjectType type) {
	if (currentProject) {
		for (auto object : currentProject->objects) {
			if (name==object->getName() && object->type == type) {
				return object;
			}
		}
	}
	return nullopt;
}

OpObj* linear(OpObj*(*popFn) ()) {
	NumberObj* zObj = static_cast<NumberObj*>(popFn());
	NumberObj* xObj = static_cast<NumberObj*>(popFn());
	StringObj* nameObj = static_cast<StringObj*>(popFn());

	optional<string> name = nameObj->value;
	optional<double> x = xObj->value;
	optional<double> z = zObj->value;

	delete nameObj;
	delete xObj;
	delete zObj;

	if (name == nullopt) return new NumberObj(nullopt, false);

	optional<CChartObject*> chart = findChart(*name, ChartObjectType::LinearChart);
	if (x == nullopt || z == nullopt || chart==nullopt) {
		return new NumberObj(nullopt, false);
	}

	CLinearChart* linearChart = static_cast<CLinearChart*>(*chart);

	return new NumberObj(linearChart->figureChart(*x, *z), false);
}
OpObj* clamp(OpObj*(*popFn) ()) {
	NumberObj* valueObj = static_cast<NumberObj*>(popFn());
	NumberObj* xObj = static_cast<NumberObj*>(popFn());
	StringObj* nameObj = static_cast<StringObj*>(popFn());

	optional<string> name = nameObj->value;
	optional<double> x = xObj->value;
	optional<double> value = valueObj->value;

	delete nameObj;
	delete xObj;
	delete valueObj;

	if (name == nullopt) return new NumberObj(nullopt, false);

	optional<CChartObject*> chart = findChart(*name, ChartObjectType::ClampChart);
	if (x == nullopt || value == nullopt || chart == nullopt) {
		return new NumberObj(nullopt, false);
	}

	CClampChart* clampChart = static_cast<CClampChart*>(*chart);

	return new NumberObj(clampChart->figureClamp(*x, *value), false);
}
OpObj* poly(OpObj*(*popFn) ()) {
	NumberObj* yObj = static_cast<NumberObj*>(popFn());
	NumberObj* xObj = static_cast<NumberObj*>(popFn());
	StringObj* nameObj = static_cast<StringObj*>(popFn());

	optional<string> name = nameObj->value;
	optional<double> x = xObj->value;
	optional<double> y = yObj->value;

	delete nameObj;
	delete xObj;
	delete yObj;

	if (name == nullopt) return new NumberObj(nullopt, false);

	optional<CChartObject*> chart = findChart(*name, ChartObjectType::PolyChart);
	if (x == nullopt || y == nullopt || chart == nullopt) {
		return new NumberObj(nullopt, false);
	}

	CPolyChart* polyChart = static_cast<CPolyChart*>(*chart);

	return new NumberObj(polyChart->hitTestPoly(*x, *y), false);
}
OpObj* trend(OpObj*(*popFn) ()) {
	NumberObj* exitObj = static_cast<NumberObj*>(popFn());
	NumberObj* entryObj = static_cast<NumberObj*>(popFn());
	NumberObj* valObj = static_cast<NumberObj*>(popFn());
	StringObj* nameObj = static_cast<StringObj*>(popFn());

	optional<string> name = nameObj->value;
	optional<double> val = valObj->value;
	optional<double> entry = entryObj->value;
	optional<double> exit = exitObj->value;

	delete nameObj;
	delete valObj;
	delete entryObj;
	delete exitObj;

	if (name == nullopt) return new NumberObj(nullopt, false);

	optional<CChartObject*> chart = findChart(*name, ChartObjectType::TrendChart);
	if (val == nullopt || entry == nullopt || exit == nullopt || chart==nullopt) {
		return new NumberObj(nullopt, false);
	}

	CTrendChart* trendChart = static_cast<CTrendChart*>(*chart);

	return new NumberObj(trendChart->figureTrend(*val, *entry, *exit), false);
}

OpObj* alert(OpObj*(*popFn) ()) {
	StringObj* msgObj = static_cast<StringObj*>(popFn());
	optional<string> msg = msgObj->value;
	delete msgObj;

	if (msg!=nullopt) {
		MessageBoxA(GetActiveWindow(), (*msg).c_str(), "Alert", 0);
	} else {
		MessageBoxA(GetActiveWindow(), "empty alert", "Alert", 0);
	}

	return new BoolObj(true, false);
}

vector<ExternalDef> InterpreterCPP::buildExternList(ChartProject* chartProject, CChartObject* thisChartObject) {
	vector<ExternalDef> externs = {
		ExternalDef("runLinear", IdentityType::Double, { IdentityType::String, IdentityType::Double, IdentityType::Double }, &linear),
		ExternalDef("runClamp", IdentityType::Double, { IdentityType::String, IdentityType::Double, IdentityType::Double }, &clamp),
		ExternalDef("runPoly", IdentityType::Double, { IdentityType::String, IdentityType::Double, IdentityType::Double }, &poly),
		ExternalDef("runTrend", IdentityType::Double, { IdentityType::String, IdentityType::Double, IdentityType::Double, IdentityType::Double }, &trend),
		ExternalDef("alert", IdentityType::Bool, {IdentityType::String}, &alert)
	};

	if (chartProject) {
		for (size_t i = 0; i < chartProject->constants.size(); i++) {
			NumberObj* numObj = new NumberObj(chartProject->constants[i]->result, true);
			externs.push_back(ExternalDef(chartProject->constants[i]->getName(), IdentityType::Double, numObj));
		}
		if (thisChartObject && thisChartObject->type != ChartObjectType::Input) {
			for (size_t i = 0; i < chartProject->inputs.size(); i++) {
				NumberObj* numObj = new NumberObj(chartProject->inputs[i]->result, true);
				externs.push_back(ExternalDef(chartProject->inputs[i]->getName(), IdentityType::Double, numObj));
			}
			for (size_t i = 0; i < chartProject->objects.size(); i++) {
				if (chartProject->objects[i] == thisChartObject) {
					break;
				}
				NumberObj* numObj = new NumberObj(chartProject->objects[i]->result, true);
				externs.push_back(ExternalDef(chartProject->objects[i]->getName(), IdentityType::Double, numObj));
			}
		}
	}

	return externs;
}

void InterpreterCPP::cleanUpExternList(vector<ExternalDef>& externals) {
	for (auto obj : externals) {
		if (obj.type != IdentityType::Function) {
			delete obj.opObj;
		}
	}
}

optional<string> InterpreterCPP::checkCompile(ChartProject* chartProject, CChartObject* thisChartObject, string code, bool isInput) {
	vector<ExternalDef> externs = buildExternList(chartProject, thisChartObject);
	
	if (isInput) {
		externs.push_back(ExternalDef("input", IdentityType::String, new StringObj("", true)));
		externs.push_back(ExternalDef("invalid", IdentityType::Bool, new BoolObj(false, false)));
	}

	try {
		Tokenizer tokenizer;
		try {
			tokenizer.tokenize(code);
			Parser parser(tokenizer.tokens);
			try {
				parser.parse(externs, IdentityType::Double);
			} catch (char e) {
				this->cleanUpExternList(externs);
				return "Parser error: " + parser.errorMsg;
			}
		} catch (char e) {
			this->cleanUpExternList(externs);
			return "Tokenizer error: " + tokenizer.errorMsg;
		}
	} catch (...) {
		this->cleanUpExternList(externs);
		return "Unknown error";
	}

	this->cleanUpExternList(externs);
	return nullopt;
}

void InterpreterCPP::runCode(ChartProject* chartProject, CScript& thisChartObject, bool* errorOccured, string* errorOut) {
	vector<ExternalDef> externs = buildExternList(chartProject, &thisChartObject);

	try {
		Tokenizer tokenizer;

		if (errorOccured) *errorOccured = false;

		try {
			tokenizer.tokenize(thisChartObject.code);
			Parser parser(tokenizer.tokens);
			try {
				parser.parse(externs, IdentityType::Double);

				currentProject = chartProject;
				OpObj* retObj = parser.program.execute(externs);

				if (retObj) {
					if (retObj->objType == OpObjType::Number) {
						thisChartObject.result = static_cast<NumberObj*>(retObj)->value;
					} else {
						thisChartObject.result = nullopt;
					}
					delete retObj;
				} else {
					thisChartObject.result = nullopt;
				}

			} catch (char e) {
				thisChartObject.result = nullopt;
				if (e == 'P') {
					if (errorOccured) *errorOccured = true;
					if (errorOut) *errorOut = "Parser error: " + parser.errorMsg;
				} else if (e == 'E') {
					if (errorOccured) *errorOccured = true;
					if (errorOut) *errorOut = "Execution error: " + parser.program.errorMsg;
				} else {
					if (errorOccured) *errorOccured = true;
					if (errorOut) *errorOut = "Unknown error";
				}
			}
		} catch (char e) {
			thisChartObject.result = nullopt;
			if (errorOccured) *errorOccured = true;
			if (errorOut) "Tokenizer error: " + tokenizer.errorMsg;
		}
	} catch (...) {
		thisChartObject.result = nullopt;
		if (errorOccured) *errorOccured = true;
		if (errorOut) *errorOut = "Unknown catch all error";
	}
	this->cleanUpExternList(externs);
}


void InterpreterCPP::runCode(ChartProject* chartProject, CInput& thisChartObject, bool* errorOccured, string* errorOut) {
	vector<ExternalDef> externs = buildExternList(chartProject, &thisChartObject);

	externs.push_back(ExternalDef("input", IdentityType::String, new StringObj(thisChartObject.input, true)));

	BoolObj* isInvalidObj = new BoolObj(false, false);
	externs.push_back(ExternalDef("invalid", IdentityType::Bool, isInvalidObj));

	try {
		Tokenizer tokenizer;

		if (errorOccured) *errorOccured = false;

		try {
			tokenizer.tokenize(thisChartObject.code);
			Parser parser(tokenizer.tokens);
			try {
				parser.parse(externs, IdentityType::Double);

				currentProject = chartProject;
				OpObj* retObj = parser.program.execute(externs);

				thisChartObject.invalid = isInvalidObj->value==nullopt ? false : *isInvalidObj->value;

				if (retObj) {
					if (retObj->objType == OpObjType::Number) {
						thisChartObject.result = static_cast<NumberObj*>(retObj)->value;
					} else {
						thisChartObject.invalid = true;
						thisChartObject.result = nullopt;
					}
					delete retObj;
				} else {
					thisChartObject.invalid = true;
					thisChartObject.result = nullopt;
				}

			} catch (char e) {
				thisChartObject.invalid = true;
				thisChartObject.result = nullopt;
				if (e == 'P') {
					if (errorOccured) *errorOccured = true;
					if (errorOut) *errorOut = "Parser error: " + parser.errorMsg;
				} else if (e == 'E') {
					if (errorOccured) *errorOccured = true;
					if (errorOut) *errorOut = "Execution error: " + parser.program.errorMsg;
				} else {
					if (errorOccured) *errorOccured = true;
					if (errorOut) *errorOut = "Unknown error";
				}
			}
		} catch (char e) {
			thisChartObject.invalid = true;
			thisChartObject.result = nullopt;
			if (errorOccured) *errorOccured = true;
			if (errorOut) "Tokenizer error: " + tokenizer.errorMsg;
		}
	} catch (...) {
		thisChartObject.invalid = true;
		thisChartObject.result = nullopt;
		if (errorOccured) *errorOccured = true;
		if (errorOut) *errorOut = "Unknown catch all error";
	}
	this->cleanUpExternList(externs);
}
