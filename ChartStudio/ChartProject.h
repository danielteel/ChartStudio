#pragma once
#include "stdafx.h"
#include <string>
#include <vector>
#include <optional>
#include "TPoint.h"
#include "helpers.h"

using namespace std;

class TLine;

class CChartObject;
class CConstant;
class CInput;
class CChartBase;
class CLinearChart;
class CTrendChart;
class CClampChart;
class CPolyChart;
class CTable;
class CTableEntry;
class CScript;

class ChartImage;
class FileParser;

struct LineDisplaySettings {
	double lineSize;
	double nodeSize;
	COLORREF lineColor;
	COLORREF nodeColor;

	LineDisplaySettings(double lineSize, double nodeSize, COLORREF lineColor, COLORREF nodeColor) {
		this->lineSize = lineSize;
		this->nodeSize = nodeSize;
		this->lineColor = lineColor;
		this->nodeColor = nodeColor;
	}

	string toString() {
		return "(" + trimmedDoubleToString(lineSize) + "," + trimmedDoubleToString(nodeSize) + "," + to_string(lineColor) + "," + to_string(nodeColor) + ")";
	}
};


struct ViewState {
	double zoom;
	TPoint xy;
	bool smoothImage;
	bool scaleDrawing;
	ViewState(double zoom, double x, double y, bool smoothImage, bool scaleDrawing) {
		this->zoom = zoom;
		this->xy = TPoint(x, y);
		this->smoothImage = smoothImage;
		this->scaleDrawing = scaleDrawing;
	}

	string toString() {
		return "viewstate(" + trimmedDoubleToString(zoom) + "," + xy.toString() + "," + to_string(smoothImage) + "," + to_string(scaleDrawing) + ")";
	}
};

class ChartProject {
public:
	ChartProject(string file);
	ChartProject();
	~ChartProject();

	void writeStringToFile(FILE * file, string str);

	
	string getFileName();
	bool hasSaveLocation();

	//methods
	bool save();
	bool saveToFile(string file);
	bool saveAs(string file);


	ChartImage* addImage(string file);
	void deleteImage(ChartImage * image);
	string nameFromFile(string file);
	string stripExtension(string filename);
	double getNewXForImage();
	void sortChartImages();

	bool isLineBad(CChartBase * chart, TLine * line);

	void validateInputs();
	void makeObjectNamesUnique();
	CChartObject* getObjectByName(string name);


	bool canMoveChartUp(CChartObject * object);
	bool canMoveChartDown(CChartObject * object);
	bool moveChartUp(CChartObject * object);
	bool moveChartDown(CChartObject * object);

	bool canMoveInputUp(CInput * input);
	bool canMoveInputDown(CInput * input);
	bool moveInputUp(CInput * input);
	bool moveInputDown(CInput * input);

	bool canMoveConstantUp(CConstant * constant);
	bool canMoveConstantDown(CConstant * constant);
	bool moveConstantUp(CConstant * constant);
	bool moveConstantDown(CConstant * constant);

	vector<CChartObject*> objects; 
	vector<CInput*> inputs;
	vector<CConstant*> constants; 

	vector<ChartImage*> chartImages;


	int viewPrecision = 2;
	int generatePrecision = 3;

	ViewState viewState = ViewState(1.0f, 0, 0, false, true);//save
	LineDisplaySettings selectedSettings = LineDisplaySettings(2.0f, 3.0f, RGB(0, 220, 0), RGB(0, 220, 0)); //save
	LineDisplaySettings notSelectedSettings = LineDisplaySettings(1.0f, 2.0f, RGB(255, 0, 0), RGB(255, 0, 0)); //save
	LineDisplaySettings notActiveSettings = LineDisplaySettings(1.0f, 1.0f, RGB(128, 128, 128), RGB(128, 128, 128));//save

	bool isValidInput(CChartObject * chart, CChartObject * input);

	CChartObject* addChart(CChartObject* chart);
	void deleteChart(CChartObject* chart);


	void makeObjectNameUnique(CChartObject * object);
	CTableEntry * newTableEntry(CTable * table, CChartObject * reference, double value);
	void deleteTableEntry(CTable * table, CTableEntry * deleteThisEntry);

	CConstant * newConstant(string name, double value);
	void deleteConstant(CConstant * constant);

	CInput * newInput(string name, string initialValue, string code);
	void deleteInput(CInput * input);

	void calculate();

	string fileName;

	template<class T> bool getItemNeighbors(vector<T*> *list, T* item, T*& prevItem, T*& nextItem);
	template<class T> bool moveItemUp(vector<T*> *list, T* itemToMove);
	template<class T> bool moveItemDown(vector<T*> *list, T * itemToMove);
};