#pragma once
#include <string>
#include <stdio.h>
using namespace std;

class TLine;
class ChartProject;
struct BoundSettings;
struct LineDisplaySettings;

class FileParser {
public:
	FileParser(string filename);
	FileParser();
	~FileParser();

	bool open(string filename);
	void close();

	string filename;


	//Returns false if unable to read/incorrect token
	bool getChar();
	void skipWhite();

	bool getIdent(string& ident);
	bool getString(string& stringVal); //gets the decoded value between -> 's, but not the 's
	bool getDouble(double& doubleVal);
	bool getBool(bool& boolVal);
	bool getSize(size_t& size);
	bool getDWORD(DWORD& dword);
	bool getInt(int& num);
	bool getData(char* pointer, size_t length);
	TLine* readInLine();
	bool readInLineDisplaySettings(LineDisplaySettings & lds);
	bool readInBounds(BoundSettings & bs);
	bool readInImage(ChartProject * chartProject);
	bool matchChar(int chr);
	bool isNotEOF();
	void setError(string msg);

	string errorString="";
	bool hasError = false;

	int look;

private:
	FILE* file=nullptr;
};

