#include "stdafx.h"
#include "Exporter.h"

#include <string>
#include <vector>
#include <stack>
#include <optional>

#include "helpers.h"

#include "ChartProject.h"
#include "CChartBase.h"
#include "CChartObject.h"
#include "CConstant.h"
#include "CInput.h"
#include "CLinearChart.h"
#include "CPolyChart.h"
#include "CScript.h"
#include "CTable.h"
#include "CClampChart.h"
#include "CTrendChart.h"
#include "TLine.h"
#include "TPoint.h"
#include "TRect.h"
#include "TSegment.h"

using namespace std;


Exporter::Exporter() {
}

void Exporter::writeStringToFile(FILE* file, string str) {
	fwrite(str.c_str(), str.length(), 1, file);
}

void Exporter::writeLine(FILE* file, TLine* line) {
	if (!line) {
		writeStringToFile(file, "L(0)");
		return;
	}
	if (line->points.size()==0) {
		writeStringToFile(file, "L(0)");
		return;
	}

	writeStringToFile(file, "L("+trimmedDoubleToString(line->getValue())+",");
	for (unsigned int i = 0; i < line->points.size(); i++) {
		writeStringToFile(file, "P("+ trimmedDoubleToString(line->points[i]->x, 3)+","+ trimmedDoubleToString(line->points[i]->y, 3)+")");
		if (i < line->points.size() - 1) {
			writeStringToFile(file, ",");
		}
	}
	writeStringToFile(file, ")");
}

void Exporter::writeLines(FILE* file, CChartBase* chart) {
	if (!chart) return;
	chart->setCalcLines();
	for (unsigned int i = 0; i < chart->calcLines.size(); i++) {
		writeLine(file, chart->calcLines[i]);
		if (i < chart->calcLines.size() - 1) {
			writeStringToFile(file, ",");
		}
	}
	chart->deleteCalcLines();
}

void Exporter::writeCode(FILE* file, string code) {
	char curChar = 0;
	for (unsigned int i = 0; i < code.length(); i++) {
		curChar = code[i];
		if (curChar == '\'') {
			fwrite("\\", 1, 1, file);
		}
		if (curChar >= ' ') {
			fwrite(&curChar, 1, 1, file);
		}
	}
}

string Exporter::getJSName(CChartObject* object, bool withThis = false) {
	if (!object) return "";
	if (withThis) {
		if (object->exportResult || object->toInput()) {
			return "T.$" + object->getName();
		}
		return "T._$" + to_string(object->tempIndex);
	}
	if (object->exportResult || object->toInput()) {
		return "$" + object->getName();
	}

	return "_$" + to_string(object->tempIndex);
}

void Exporter::setUniqueIDs(ChartProject* chartProject) {
	unsigned int i = 1;
	for (auto & constant : chartProject->constants) {
		constant->tempIndex = i++;
	}
	for (auto & input : chartProject->inputs) {
		input->tempIndex = i++;
	}
	for (auto & object : chartProject->objects) {
		object->tempIndex = i++;
	}
}

bool Exporter::exportToJS(ChartProject* chartProject, string toFile) {
	if (chartProject == nullptr) return false;

	FILE* saveFile = NULL;
	string buff;

	toFile = trim(toFile);
	if (toFile.length() == 0) return false;
	if (fopen_s(&saveFile, toFile.c_str(), "wb")) return false;

	chartProject->makeObjectNamesUnique();
	setUniqueIDs(chartProject);

	writeStringToFile(saveFile, "import {Point,Line,LinearChart,ClampChart,TrendChart,PolyChart,ChartTableEntry,ChartTable,ChartInput,ChartScript,ChartConstant} from './ChartObjects';\n\n");
	writeStringToFile(saveFile, "export default function ChartProject(){\n");
	writeStringToFile(saveFile, "\tconst O=true;\n");
	writeStringToFile(saveFile, "\tconst F=false;\n");
	writeStringToFile(saveFile, "\tconst T=this;\n");
	writeStringToFile(saveFile, "\tconst P=(x,y)=>{return new Point(x,y);};\n");
	writeStringToFile(saveFile, "\tconst L=(val,...points)=>{return new Line(points,val);};\n");
	for (auto & constant : chartProject->constants) {
		writeStringToFile(saveFile, "\t"+getJSName(constant, true));
		writeStringToFile(saveFile, "=new ChartConstant('");
		writeStringToFile(saveFile, constant->getName());
		writeStringToFile(saveFile, "',");
		writeStringToFile(saveFile, constant->result ? trimmedDoubleToString(*constant->result) : "0");
		writeStringToFile(saveFile, ");\n");
	}

	for (auto & input : chartProject->inputs) {
		writeStringToFile(saveFile, "\t"+getJSName(input, true));
		writeStringToFile(saveFile, "=new ChartInput('");
		writeStringToFile(saveFile, input->getName());
		writeStringToFile(saveFile, "','");
		writeCode(saveFile, input->getCode());
		writeStringToFile(saveFile, "','");
		writeCode(saveFile, input->input);
		writeStringToFile(saveFile, "');\n");
	}

	for (auto & object : chartProject->objects) {

		switch (object->type) {
		case ChartObjectType::ClampChart: 
			{
				CClampChart* clamp = object->toClampChart();
				if (clamp) {
					writeStringToFile(saveFile, "\t"+ getJSName(object, true));
					writeStringToFile(saveFile, "=new ClampChart('");
					writeStringToFile(saveFile, object->getName());
					writeStringToFile(saveFile, "',");
					writeStringToFile(saveFile, clamp->yIsInputAxis?"O,":"F,");
					writeStringToFile(saveFile, getJSName(clamp->inputVariable1, true));
					writeStringToFile(saveFile, ",");
					writeStringToFile(saveFile, getJSName(clamp->inputVariable2, true));
					writeStringToFile(saveFile, ",");
					clamp->setCalcLines();
					writeLine(saveFile, clamp->calcMinLine);
					writeStringToFile(saveFile, ",");
					writeLine(saveFile, clamp->calcMaxLine);
					clamp->deleteCalcLines();
					writeStringToFile(saveFile, ");\n");
				}
				break;
			}
		case ChartObjectType::LinearChart:
			{
				CLinearChart* linear = object->toLinearChart();
				if (linear) {
					writeStringToFile(saveFile, "\t" + getJSName(object, true));
					writeStringToFile(saveFile, "=new LinearChart('");
					writeStringToFile(saveFile, object->getName());
					writeStringToFile(saveFile, "',");
					writeStringToFile(saveFile, linear->yIsInputAxis ? "O," : "F,");
					writeStringToFile(saveFile, getJSName(linear->inputVariable1, true));
					writeStringToFile(saveFile, ",");
					writeStringToFile(saveFile, getJSName(linear->inputVariable2, true));
					writeStringToFile(saveFile, ",");
					writeLines(saveFile, linear);
					writeStringToFile(saveFile, ");\n");
				}
				break;
			}
		case ChartObjectType::TrendChart:
			{
				CTrendChart* trend = object->toTrendChart();
				if (trend) {
					writeStringToFile(saveFile, "\t" + getJSName(object, true));
					writeStringToFile(saveFile, "=new TrendChart('");
					writeStringToFile(saveFile, object->getName());
					writeStringToFile(saveFile, "',");
					writeStringToFile(saveFile, trend->yIsInputAxis ? "O," : "F,");
					writeStringToFile(saveFile, getJSName(trend->inputVariable1, true));
					writeStringToFile(saveFile, ",");
					writeStringToFile(saveFile, getJSName(trend->entryPointInput, true));
					writeStringToFile(saveFile, ",");
					writeStringToFile(saveFile, getJSName(trend->inputVariable2, true));
					writeStringToFile(saveFile, ",");
					writeLines(saveFile, trend);
					writeStringToFile(saveFile, ");\n");
				}
				break;
			}
			break;
		case ChartObjectType::PolyChart:
			{
				CPolyChart* poly = object->toPolyChart();
				if (poly) {
					writeStringToFile(saveFile, "\t" + getJSName(object, true));
					writeStringToFile(saveFile, "=new PolyChart('");
					writeStringToFile(saveFile, object->getName());
					writeStringToFile(saveFile, "',");
					writeStringToFile(saveFile, getJSName(poly->inputVariable1, true));
					writeStringToFile(saveFile, ",");
					writeStringToFile(saveFile, getJSName(poly->inputVariable2, true));
					writeStringToFile(saveFile, ",");
					writeLines(saveFile, poly);
					writeStringToFile(saveFile, ");\n");
				}
				break;
			}
		case ChartObjectType::Script:
			{
				CScript* script = object->toScript();
				if (script) {
					writeStringToFile(saveFile, "\t" + getJSName(object, true));
					writeStringToFile(saveFile, "=new ChartScript('");
					writeStringToFile(saveFile, object->getName());
					writeStringToFile(saveFile, "','");
					writeCode(saveFile, script->getCode());
					writeStringToFile(saveFile, "');\n");
					break;
				}
			}
		case ChartObjectType::Table:
			{
				CTable* table = object->toTable();
				if (table) {
					writeStringToFile(saveFile, "\t" + getJSName(object, true));
					writeStringToFile(saveFile, "=new ChartTable('");
					writeStringToFile(saveFile, object->getName());
					writeStringToFile(saveFile, "',");
					writeStringToFile(saveFile, getJSName(table->inputVariable, true));
					writeStringToFile(saveFile, ",");

					for (unsigned int i = 0; i < table->table.size(); i++) {
						writeStringToFile(saveFile, "new ChartTableEntry(");
						writeStringToFile(saveFile, getJSName(table->table[i]->chartObject, true));
						writeStringToFile(saveFile, ",");
						writeStringToFile(saveFile, trimmedDoubleToString(table->table[i]->getValue()));
						writeStringToFile(saveFile, ")");
						if (i < table->table.size() - 1) {
							writeStringToFile(saveFile, ",");
						}
					}
					writeStringToFile(saveFile, ");\n");
				}
				break;
			}
		}
	}

	writeStringToFile(saveFile, "\tT.calcArray=[");

	for (auto & constant : chartProject->constants) {
		writeStringToFile(saveFile, getJSName(constant, true)+",");
	}
	for (auto & input : chartProject->inputs) {
		writeStringToFile(saveFile, getJSName(input, true) + ",");
	}
	for (auto & object : chartProject->objects) {
		writeStringToFile(saveFile, getJSName(object, true) + ",");
	}
	writeStringToFile(saveFile, "];\n");

	writeStringToFile(saveFile, "\tT.exports=[");
	for (auto & input : chartProject->inputs) {
		writeStringToFile(saveFile, getJSName(input, true) + ",");
	}
	for (auto & object : chartProject->objects) {
		if (object->exportResult) writeStringToFile(saveFile, getJSName(object, true) + ",");
	}
	writeStringToFile(saveFile, "];\n");
	writeStringToFile(saveFile, "\tT.alerts=[];\n");
	writeStringToFile(saveFile, "\tT.calcInputs=()=>{T.alerts.length=0; for (let obj of T.calcArray) {if (obj.type==='input') obj.calc(T.calcArray, T.alerts);}};\n");
	writeStringToFile(saveFile, "\tT.calc=()=>{T.alerts.length=0; for (let obj of T.calcArray) {obj.calc(T.calcArray, T.alerts);}};\n}");
	fclose(saveFile);
	return true;
}

Exporter::~Exporter() {
}