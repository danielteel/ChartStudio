#pragma once
#include <string>
class ChartProject;
class TLine;
class CChartBase;
class CChartObject;

class Exporter {
private:

	
public:
	Exporter();
	void writeLine(FILE * file, TLine * line);
	void writeLines(FILE * file, CChartBase * chart);
	void writeStringToFile(FILE * file, std::string str);
	void writeCode(FILE * file, std::string code);
	std::string getJSName(CChartObject * object, bool withThis);
	void setUniqueIDs(ChartProject * chartProject);
	bool exportToJS(ChartProject* chartProject, std::string toFile);
	~Exporter();
};

