#include <algorithm>
#include <stdlib.h>
#include <stdio.h>

#include "stdafx.h"

#include "helpers.h"
#include "FileParser.h"

#include "ChartImage.h"

#include "TLine.h"

#include "CChartObject.h"
#include "CConstant.h"
#include "CInput.h"
#include "CChartBase.h"
#include "CLinearChart.h"
#include "CTrendChart.h"
#include "CClampChart.h"
#include "CPolyChart.h"
#include "CScript.h"
#include "CTable.h"

#include "ChartProject.h"

ChartProject::ChartProject(string file) {
	//load from file

	file = trim(file);
	this->fileName = file;


	FileParser parser;
	if (!parser.open(file)) {
		MessageBox(GetActiveWindow(), "Unable to open the file", "Error", MB_ICONERROR);
	}

	string ident;
	while (parser.isNotEOF()) {
		if (parser.getIdent(ident)) {
			stringToLower(ident);
			if (ident == "sel") {
				if (!parser.readInLineDisplaySettings(this->selectedSettings)) break;

			} else if (ident == "unsel") {
				if (!parser.readInLineDisplaySettings(this->notSelectedSettings)) break;

			} else if (ident == "not") {
				if (!parser.readInLineDisplaySettings(this->notActiveSettings)) break;

			} else if (ident == "viewprec") {
				if (!parser.matchChar('(')) break;
				if (!parser.getInt(this->viewPrecision)) break;
				if (!parser.matchChar(')')) break;

			} else if (ident == "genprec") {
				if (!parser.matchChar('(')) break;
				if (!parser.getInt(this->generatePrecision)) break;
				if (!parser.matchChar(')')) break;

			}else if (ident == "viewstate"){
				if (!parser.matchChar('(')) break;
				if (!parser.getDouble(this->viewState.zoom)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.matchChar('(')) break;
				if (!parser.getDouble(this->viewState.xy.x)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getDouble(this->viewState.xy.y)) break;
				if (!parser.matchChar(')')) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getBool(this->viewState.smoothImage)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getBool(this->viewState.scaleDrawing)) break;
				if (!parser.matchChar(')')) break;

			} else if (ident == "image") {
				if (!parser.readInImage(this)) break;

			} else if (ident == "input") {
				string name, code;
				string value;
				if (!parser.matchChar('(')) break;
				if (!parser.getString(name)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getString(value)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getString(code)) break;
				if (!parser.matchChar(')')) break;
				CInput* newInput=this->newInput(name, value, code);

			} else if (ident == "constant") {
				string name;
				double value;
				if (!parser.matchChar('(')) break;
				if (!parser.getString(name)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getDouble(value)) break;
				if (!parser.matchChar(')')) break;
				this->newConstant(name, value);

			} else if (ident == "clamp") {
				string name, input1, input2;
				TLine* maxLine, *minLine;
				bool exportResult, yIsInputAxis;
				BoundSettings bounds;

				if (!parser.matchChar('(')) break;
				if (!parser.getString(name)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getBool(exportResult)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getBool(yIsInputAxis)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.readInBounds(bounds)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getString(input1)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getString(input2)) break;
				if (!parser.matchChar(',')) break;

				maxLine = parser.readInLine();
				if (!maxLine) break;
				minLine = parser.readInLine();
				if (!minLine) break;
				this->addChart(new CClampChart(name, exportResult, yIsInputAxis, bounds, input1, input2, minLine, maxLine));
				if (!parser.matchChar(')')) break;

			} else if (ident == "linear") {
				string name, input1, input2;
				bool exportResult, yIsInputAxis;
				BoundSettings bounds;

				if (!parser.matchChar('(')) break;
				if (!parser.getString(name)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getBool(exportResult)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getBool(yIsInputAxis)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.readInBounds(bounds)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getString(input1)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getString(input2)) break;
				if (!parser.matchChar(',')) break;

				CLinearChart* newLinear = new CLinearChart(name, exportResult, yIsInputAxis, bounds, input1, input2, {});
				this->addChart(newLinear);

				while (parser.isNotEOF() && parser.look != ')' && parser.look!=',') {
					TLine* newLine = parser.readInLine();
					if (newLine) {
						newLinear->addLine(newLine);
					} else {
						break;
					}
				}				
				if (parser.look == ',') {
					bool reverseInterpolate;
					if (!parser.matchChar(',')) break;
					if (!parser.getBool(reverseInterpolate)) break;
					newLinear->reverseInterpolate = reverseInterpolate;
				}
				if (!parser.matchChar(')')) break;

			} else if (ident == "poly") {
				string name, input1, input2;
				bool exportResult;
				BoundSettings bounds;

				if (!parser.matchChar('(')) break;
				if (!parser.getString(name)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getBool(exportResult)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.readInBounds(bounds)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getString(input1)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getString(input2)) break;
				if (!parser.matchChar(',')) break;

				CPolyChart* newPoly = new CPolyChart(name, exportResult, bounds, input1, input2, {});
				this->addChart(newPoly);

				while (parser.isNotEOF() && parser.look != ')') {
					TLine* newLine = parser.readInLine();
					if (newLine) {
						newPoly->addLine(newLine);
					} else {
						break;
					}
				}
				if (!parser.matchChar(')')) break;

			} else if (ident == "trend") {
				string name, input1, input2, input3;
				bool exportResult, yIsInputAxis;
				BoundSettings bounds;

				if (!parser.matchChar('(')) break;
				if (!parser.getString(name)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getBool(exportResult)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getBool(yIsInputAxis)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.readInBounds(bounds)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getString(input1)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getString(input2)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getString(input3)) break;
				if (!parser.matchChar(',')) break;

				CTrendChart* newTrend = new CTrendChart(name, exportResult, yIsInputAxis, bounds, input1, input2, input3, {});
				this->addChart(newTrend);

				while (parser.isNotEOF() && parser.look != ')') {
					TLine* newLine = parser.readInLine();
					if (newLine) {
						newTrend->addLine(newLine);
					} else {
						break;
					}
				}
				if (!parser.matchChar(')')) break;

			} else if (ident == "script") {
				string name, code;
				bool exportResult;
				if (!parser.matchChar('(')) break;
				if (!parser.getString(name)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getBool(exportResult)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getString(code)) break;

				CScript* newScript = new CScript(name, exportResult, code);
				this->addChart(newScript);

				if (!parser.matchChar(')')) break;

			} else if (ident == "table") {
				string name, tableRef, input1;
				double tableVal;
				bool exportResult;

				if (!parser.matchChar('(')) break;
				if (!parser.getString(name)) break;
				if (!parser.matchChar(',')) break;
				if (!parser.getBool(exportResult)) break;

				CTable* newTable = new CTable(name, exportResult);
				this->addChart(newTable);

				if (!parser.matchChar(',')) break;

				if (!parser.getString(input1)) break;
				newTable->inputVariableLink = input1;

				if (!parser.matchChar(',')) break;

				while (parser.isNotEOF() && parser.look != ')') {
					if (!parser.matchChar('(')) break;
					if (!parser.getString(tableRef)) break;
					if (!parser.matchChar(',')) break;
					if (!parser.getDouble(tableVal)) break;
					if (!parser.matchChar(')')) break;

					newTable->addTableEntry(new CTableEntry(tableRef, tableVal));
				}
				if (!parser.matchChar(')')) break;

			} else {
				parser.setError("Unexpected identifier");
				break;
			}

		} else {
			break;
		}
	}
	if (parser.hasError) {
		MessageBox(GetActiveWindow(), parser.errorString.c_str(), "Error loading file, but we loaded as much as we could", 0);
	}

	parser.close();

	//Link object references
	for (auto & object : this->objects) {

		CChartBase* chartBase = object->toChartBase();
		if (chartBase) {
			chartBase->inputVariable1 = getObjectByName(chartBase->inputVariable1Link);
			chartBase->inputVariable2 = getObjectByName(chartBase->inputVariable2Link);

			CTrendChart* trendChart = object->toTrendChart();
			if (trendChart) {
				trendChart->entryPointInput = getObjectByName(trendChart->entrpointInputLink);
			}
		}

		CTable* tableObject = object->toTable();
		if (tableObject) {
			tableObject->inputVariable = getObjectByName(tableObject->inputVariableLink);
			for (auto & tableItem : tableObject->table) {
				tableItem->chartObject = getObjectByName(tableItem->chartObjectLink);
			}
		}
	}

	this->sortChartImages();
}

ChartProject::ChartProject() {
}

ChartProject::~ChartProject() {
	for (auto & current : this->objects) {
		delete current;
	}
	for (auto & current : this->inputs) {
		delete current;
	}
	for (auto & current : this->constants) {
		delete current;
	}
	for (auto & current : this->chartImages) {
		delete current;
	}
}

void ChartProject::writeStringToFile(FILE* file, string str) {
	fwrite(str.c_str(), str.length(), 1, file);
}

bool ChartProject::saveToFile(string file) {
	file = trim(file);
	if (file.length() == 0) {
		return false;
	}
	FILE* saveFile = NULL;

	if (fopen_s(&saveFile, file.c_str(), "wb")){
		return false;
	}

	this->makeObjectNamesUnique();

	writeStringToFile(saveFile, "sel" + this->selectedSettings.toString());
	writeStringToFile(saveFile, "unsel" + this->notSelectedSettings.toString());
	writeStringToFile(saveFile, "not" + this->notActiveSettings.toString());
	writeStringToFile(saveFile, "viewprec(" + to_string(this->viewPrecision)+")");
	writeStringToFile(saveFile, "genprec(" + to_string(this->generatePrecision)+")");
	writeStringToFile(saveFile, this->viewState.toString());

	for (auto & input : this->inputs) {
		writeStringToFile(saveFile, input->toString());
	}
	for (auto & constant : this->constants) {
		writeStringToFile(saveFile, constant->toString());
	}
	for (auto & object : this->objects) {
		writeStringToFile(saveFile, object->toString());
	}

	for (auto & image : this->chartImages) {
		char* imgData = nullptr;
		size_t imgDataLen = 0;
		writeStringToFile(saveFile, image->toString(&imgData, &imgDataLen, 90));
		if (imgData) {
			fwrite(imgData, imgDataLen, 1, saveFile);
			delete[] imgData;
			imgData = nullptr;
		}
		writeStringToFile(saveFile, ")");
	}

	fclose(saveFile);
	return true;
}

bool ChartProject::save() {
	if (this->hasSaveLocation()) {
		return this->saveToFile(this->fileName);
	}
	return false;
}

bool ChartProject::saveAs(string file) {
	this->fileName = file;
	if (!file.empty()) {
		return this->saveToFile(file);
	}
	return false;
}

string ChartProject::getFileName() {
	return this->fileName;
}

bool ChartProject::hasSaveLocation() {
	return !this->fileName.empty();
}

string ChartProject::nameFromFile(string file) {
	return file.substr(file.find_last_of("/\\")+1);
}

string ChartProject::stripExtension(string filename) {
	size_t index=filename.find_last_of(".");
	return filename.substr(0, filename.find_last_of("."));
}

double ChartProject::getNewXForImage() {
	double x = 0;
	for (auto & image : this->chartImages) {
		if (image->getPosition().x + image->getSize().x > x){
			x = image->getPosition().x + image->getSize().x;
		}
	}
	return x;
}

void ChartProject::sortChartImages() {
	if (this->chartImages.size() < 2) return;

	bool notSwapped;
	do {
		notSwapped = true;
		for (size_t i = 0; i < this->chartImages.size() - 1; i++) {
			if (this->chartImages[i]->isLess(this->chartImages[i+1])) {
				ChartImage* swapper = this->chartImages[i];
				this->chartImages[i] = this->chartImages[i + 1];
				this->chartImages[i + 1] = swapper;
				notSwapped = false;
			}
		}
	} while (notSwapped == false);
}

ChartImage* ChartProject::addImage(string file) {
	wstring fileWide(file.begin(), file.end());
	Gdiplus::Bitmap* chartImage = new Gdiplus::Bitmap(fileWide.c_str());

	if (chartImage) {
		HBITMAP chartHBitmap = NULL;
		chartImage->GetHBITMAP(RGB(200, 200, 200), &chartHBitmap);
		delete chartImage;
		if (chartHBitmap) {
			double newX = this->getNewXForImage();
			ChartImage* newChartImage = new ChartImage(this->stripExtension(this->nameFromFile(file)), chartHBitmap, TPoint(newX,0));
			this->chartImages.push_back(newChartImage);
			this->sortChartImages();
			return newChartImage;
		}
	}
	return nullptr;
}

void ChartProject::deleteImage(ChartImage* image) {
	if (image) {
		for (auto it = this->chartImages.begin(); it != this->chartImages.end(); it++) {
			if (*it == image) {
				delete image;
				it = this->chartImages.erase(it);
				break;
			}
		}
	}
}


bool ChartProject::isLineBad(CChartBase* chart, TLine* line) {
	if (chart->type==ChartObjectType::LinearChart) {
		//linear chart
		if (line->isLinear(chart->yIsInputAxis) == false || line->points.size() < 2) {
			return true;
		}
	} else if (chart->type == ChartObjectType::PolyChart) {
		//poly chart
		if (line->points.size() < 3) {
			return true;
		}
	} else if (chart->type == ChartObjectType::TrendChart) {
		//trend chart
		if (line->isLinear(!chart->yIsInputAxis) == false || line->points.size() < 2) {
			return true;
		}
	} else if (chart->type == ChartObjectType::ClampChart) {
		//clamp chart
		if (line->points.size() > 0 && line->points.size() < 2) {
			return true;
		}
		if (line->points.size() > 0 && line->isLinear(chart->yIsInputAxis) == false) {
			return true;
		}
	}
	return false;
}


bool ChartProject::isValidInput(CChartObject* chart, CChartObject* input) {
	for (auto & constantVar : this->constants) {
		if (input == constantVar) return true;
	}
	for (auto & inputVar : this->inputs) {
		if (input == inputVar) return true;
	}

	for (size_t i = 0; i < this->objects.size(); i++) {
		if (chart == this->objects[i]) return false;
		if (input == this->objects[i]) return true;
	}
	return false;
 }

void ChartProject::validateInputs() {
	for (auto & chart : this->objects) {
		CChartBase* chartBase = chart->toChartBase();
		CTrendChart* trendChart = chart->toTrendChart();
		CTable* tableObject = chart->toTable();
		if (chartBase) {
			if (isValidInput(chartBase, chartBase->inputVariable1) == false) {
				chartBase->inputVariable1 = nullptr;
			}
			if (isValidInput(chartBase, chartBase->inputVariable2) == false) {
				chartBase->inputVariable2 = nullptr;
			}
		}
		if (trendChart) {
			if (isValidInput(trendChart, trendChart->entryPointInput) == false) {
				trendChart->entryPointInput = nullptr;
			}
		}
		if (tableObject) {
			if (isValidInput(tableObject, tableObject->inputVariable) == false) {
				tableObject->inputVariable = nullptr;
			}
			for (auto & entry : tableObject->table) {
				if (isValidInput(tableObject, entry->chartObject) == false) {
					entry->chartObject = nullptr;
				}
			}
		}
	}
}

CChartObject* ChartProject::getObjectByName(string name) {
	trim(name);
	stringToLower(name);
	for (auto & chart : this->objects) {
		string chartName = chart->getName();
		trim(chartName);
		stringToLower(chartName);
		if (chartName == name) {
			return chart;
		}
	}
	for (auto & input : this->inputs) {
		string inputName = input->getName();
		trim(inputName);
		stringToLower(inputName);
		if (inputName == name) {
			return input;
		}
	}
	for (auto & constant : this->constants) {
		string constantName = constant->getName();
		trim(constantName);
		stringToLower(constantName);
		if (constantName == name) {
			return constant;
		}
	}
	return nullptr;
}


void ChartProject::makeObjectNameUnique(CChartObject* object) {
	if (!object) return;
	if (getObjectByName(object->getName()) == object) return;

	size_t i = 1;
	string withoutTrailing = stripTrailingNumber(object->getName());
	while (getObjectByName(withoutTrailing+to_string(i)) && i<5000) {//arbitray number to prevent possible inifite loop
		i++;
	}
	object->setName(withoutTrailing + to_string(i));
}

void ChartProject::makeObjectNamesUnique() {
	for (auto & chart : this->objects) {
		makeObjectNameUnique(chart);
	}
	for (auto & input : this->inputs) {
		makeObjectNameUnique(input);
	}
	for (auto & constant : this->constants) {
		makeObjectNameUnique(constant);
	}
}

CChartObject* ChartProject::addChart(CChartObject * chart) {
	if (chart) {
		this->objects.push_back(chart);
		this->makeObjectNamesUnique();
	}
	return chart;
}

void ChartProject::deleteChart(CChartObject * chart) {
	for (auto it = this->objects.begin(); it != this->objects.end(); it++) {
		if (*it == chart) {
			delete chart;
			it = this->objects.erase(it);
			this->validateInputs();
			break;
		}
	}
}

template <class T> bool ChartProject::getItemNeighbors(vector<T*> *list, T* itemToFind, T*& prevItem, T*& nextItem) {
	prevItem = nullptr;
	nextItem = nullptr;
	bool foundItem = false;
	if (itemToFind == nullptr) return false;
	for (T* item : *list) {
		if (item == itemToFind) {
			foundItem = true;
		} else if (foundItem == false) {
			prevItem = item;
		} else if (foundItem == true) {
			nextItem = item;
			break;
		}
	}
	if (foundItem == false) {
		prevItem = nullptr;
	}
	return foundItem;
}

template <class T> bool ChartProject::moveItemUp(vector<T*> *list, T* itemToMove) {
	if (itemToMove && list && list->size() > 1) {
		if (list->front() != itemToMove) {
			for (size_t i = 1; i < list->size(); i++) {
				if ((*list)[i] == itemToMove) {
					T* prevItem = (*list)[i - 1];
					(*list)[i - 1] = (*list)[i];
					(*list)[i] = prevItem;
					validateInputs();
					return true;
				}
			}
		}
	}
	return false;
}

template <class T> bool ChartProject::moveItemDown(vector<T*> *list, T* itemToMove) {
	if (itemToMove && list && list->size() > 1) {
		if (list->back() != itemToMove) {
			for (size_t i = 0; i < list->size()-1; i++) {
				if ((*list)[i] == itemToMove) {
					T* nextItem = (*list)[i + 1];
					(*list)[i + 1] = (*list)[i];
					(*list)[i] = nextItem;
					validateInputs();
					return true;
				}
			}
		}
	}
	return false;
}

bool ChartProject::canMoveChartUp(CChartObject* object) {
	CChartObject *prevObj, *nextObj;
	this->getItemNeighbors(&this->objects, object, prevObj, nextObj);
	return prevObj != nullptr;
}

bool ChartProject::canMoveChartDown(CChartObject* object) {
	CChartObject *prevObj, *nextObj;
	this->getItemNeighbors(&this->objects, object, prevObj, nextObj);
	return nextObj != nullptr;
}

bool ChartProject::moveChartUp(CChartObject* object) {
	return this->moveItemUp(&this->objects, object);
}

bool ChartProject::moveChartDown(CChartObject* object) {
	return this->moveItemDown(&this->objects, object);
}

bool ChartProject::canMoveInputUp(CInput* input) {
	CInput *prevInput, *nextInput;
	this->getItemNeighbors(&this->inputs, input, prevInput, nextInput);
	return prevInput != nullptr;
}

bool ChartProject::canMoveInputDown(CInput* input) {
	CInput *prevInput, *nextInput;
	this->getItemNeighbors(&this->inputs, input, prevInput, nextInput);
	return nextInput != nullptr;
}

bool ChartProject::moveInputUp(CInput* input) {
	return this->moveItemUp(&this->inputs, input);
}

bool ChartProject::moveInputDown(CInput* input) {
	return this->moveItemDown(&this->inputs, input);
}

bool ChartProject::canMoveConstantUp(CConstant* constant) {
	CConstant *prevConstant, *nextConstant;
	this->getItemNeighbors(&this->constants, constant, prevConstant, nextConstant);
	return prevConstant != nullptr;
}

bool ChartProject::canMoveConstantDown(CConstant* constant) {
	CConstant *prevConstant, *nextConstant;
	this->getItemNeighbors(&this->constants, constant, prevConstant, nextConstant);
	return nextConstant != nullptr;
}

bool ChartProject::moveConstantUp(CConstant* constant) {
	return this->moveItemUp(&this->constants, constant);
}

bool ChartProject::moveConstantDown(CConstant* constant) {
	return this->moveItemDown(&this->constants, constant);
}

CTableEntry* ChartProject::newTableEntry(CTable* table, CChartObject* reference, double value) {
	if (table) {
		CTableEntry* newEntry = new CTableEntry(reference, value);
		table->addTableEntry(newEntry);
		return newEntry;
	}
	return nullptr;
}

void ChartProject::deleteTableEntry(CTable* table, CTableEntry* deleteThisEntry) {
	if (table) {
		table->deleteTableEntry(deleteThisEntry);
	}
}

CConstant* ChartProject::newConstant(string name, double value) {
	CConstant* constant = new CConstant(name,value);
	if (constant) {
		this->constants.push_back(constant);
		this->makeObjectNamesUnique();
	}
	return constant;
}

void ChartProject::deleteConstant(CConstant* constant) {
	if (constant) {
		for (auto it = this->constants.begin(); it != this->constants.end(); it++) {
			if (*it == constant) {
				delete constant;
				it = this->constants.erase(it);
				break;
			}
		}
		this->validateInputs();
	}
}

CInput* ChartProject::newInput(string name, string initialValue, string code) {
	CInput* input = new CInput(name, initialValue, code);
	if (input) {
		this->inputs.push_back(input);
		this->makeObjectNamesUnique();
	}
	return input;
}

void ChartProject::deleteInput(CInput* input) {
	if (input) {
		for (auto it = this->inputs.begin(); it != this->inputs.end(); it++) {
			if (*it == input) {
				delete input;
				it = this->inputs.erase(it);
				break;
			}
		}
		this->validateInputs();
	}
}

void ChartProject::calculate() {
	for (size_t i = 0; i < this->inputs.size(); i++) {
		this->inputs[i]->calc(this);
	}
	for (auto & object : this->objects) {
		object->calc(this);
	}
}