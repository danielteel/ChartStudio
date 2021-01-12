#include "stdafx.h"
#include "FileParser.h"
#include "ChartProject.h"
#include "ChartImage.h"
#include "helpers.h"
#include "TLine.h"
#include "TPoint.h"
#include "CChartBase.h"
#include <stdio.h>
#include <string>

FileParser::FileParser(string filename) {
	open(filename);
}

FileParser::FileParser() {
}


FileParser::~FileParser() {
	close();
}

bool FileParser::open(string filename) {
	close();
	
	hasError = false;
	errorString = "";

	filename = trim(filename);
	if (filename.length() == 0) {
		setError("Empty filename.");
		return false;
	}

	if (fopen_s(&file, filename.c_str(), "rb")) {
		file = nullptr;
		setError("Cant open '" + filename + "'!");
		return false;
	}

	this->filename = filename;

	getChar();
	skipWhite();
	return true;
}

void FileParser::close() {
	if (file) {
		fclose(file);
		file = nullptr;
	}
}

bool FileParser::getChar() {
	look = fgetc(file);
	return isNotEOF();
}

void FileParser::skipWhite() {
	while (isNotEOF() && isSpace(look)) {
		getChar();
	}
}

bool FileParser::getIdent(string & ident) {
	ident = "";
	if (isAlpha(look)) {
		while (isNotEOF() && (isAlNum(look) || look == '.' || look == '_')) {
			ident += char(look);
			getChar();
		}
	} else {
		string msg = "Expected a identifier, but got a '";
		msg += look;
		msg += "'";
		setError(msg);
		return !hasError;
	}
	skipWhite();
	return !hasError;
}

bool FileParser::getString(string & stringVal) {
	stringVal = "";
	if (look=='\'') {
		getChar();
		while (isNotEOF() && look!='\'') {
			stringVal += char(look);
			getChar();
		}
		if (!matchChar('\'')) {
			setError("End of file before we could read the rest of a string");
			return !hasError;
		}
	} else {
		string msg = "Expected a string, but got a '";
		msg += look;
		msg += "'";
		setError(msg);
		return !hasError;
	}
	stringVal = decodeString(stringVal);
	skipWhite();
	return !hasError;
}

bool FileParser::getDouble(double & doubleVal) {
	string number = "";
	doubleVal = 0;
	bool hasDec = false;
	bool hasNeg = false;
	if (isDigit(look) || look=='.' || look=='-') {
		if (look == '.') {
			hasDec = true;
			number += ".";
			getChar();
		}
		if (look == '-') {
			hasNeg = true;
			getChar();
		}
		while (isNotEOF() && (isDigit(look) || (look=='.' && hasDec==false))) {
			if (look == '.') {
				hasDec = true;
			}
			number += char(look);
			getChar();
		}
		if (hasDec && number.length() == 1) {
			setError("Expected a number, but got a lone '.'");
			return !hasError;
		}
	} else {
		string msg = "Expected a number, but got a '";
		msg += look;
		msg += "'";
		setError(msg);
		return !hasError;
	}

	doubleVal = stod(number);
	if (hasNeg) {
		doubleVal = -doubleVal;
	}
	skipWhite();
	return !hasError;
}

bool FileParser::getBool(bool & boolVal) {
	string number = "";
	boolVal = false;
	if (isDigit(look)) {
		while (isNotEOF() && isDigit(look)) {
			number += char(look);
			getChar();
		}
	} else {
		string msg = "Expected a digit, but got a '";
		msg += look;
		msg += "'";
		setError(msg);
		return !hasError;
	}

	boolVal = stoi(number) != 0;
	skipWhite();
	return !hasError;
}

bool FileParser::getSize(size_t & size) {
	string number = "";
	size = 0;
	if (isDigit(look)) {
		while (isNotEOF() && isDigit(look)) {
			number += char(look);
			getChar();
		}
	} else {
		string msg = "Expected a digit, but got a '";
		msg += look;
		msg += "'";
		setError(msg);
		return !hasError;
	}

	size = stoul(number);
	skipWhite();
	return !hasError;
}

bool FileParser::getDWORD(DWORD & dword) {
	string number = "";
	dword = 0;
	if (isDigit(look)) {
		while (isNotEOF() && isDigit(look)) {
			number += char(look);
			getChar();
		}
	} else {
		string msg = "Expected a digit, but got a '";
		msg += look;
		msg += "'";
		setError(msg);
		return !hasError;
	}
	dword = stoul(number);
	skipWhite();
	return !hasError;
}

bool FileParser::getInt(int & num) {
	string number = "";
	num = 0;
	bool hasNeg = false;
	if (isDigit(look) || look == '-') {
		if (look == '-') {
			hasNeg = true;
			getChar();
		}
		if (!isNotEOF() || !isDigit(look)) {
			setError("Expected an int, but got a lone '-'");
			return !hasError;
		}
		while (isNotEOF() && isDigit(look)) {
			number += (char)char(look);
			getChar();
		}
	} else {
		string msg = "Expected an int, but got a '";
		msg += look;
		msg += "'";
		setError(msg);
		return !hasError;
	}

	num = stoi(number);
	if (hasNeg) num = 0 - num;
	skipWhite();
	return !hasError;
}

bool FileParser::getData(char* pointer, size_t length) {
	if (fread(pointer, length, 1, file) < 1) {
		setError("Unable to read all the data");
		return !hasError;
	}
	getChar();
	skipWhite();
	return !hasError;
}



TLine* FileParser::readInLine() {
	TLine* returnLine = nullptr;
	double lineValue, x, y;
	if (!this->matchChar('(')) return nullptr;
	if (!this->getDouble(lineValue)) return nullptr;
	if (!this->matchChar(',')) return nullptr;

	returnLine = new TLine(lineValue);
	if (!returnLine) {
		setError("Unable to allocate new TLine");
		return nullptr;
	}

	while (this->isNotEOF() && this->look != ')') {
		if (!this->matchChar('(')) {
			delete returnLine;
			return nullptr;
		}
		if (!this->getDouble(x)) {
			delete returnLine;
			return nullptr;
		}
		if (!this->matchChar(',')) {
			delete returnLine;
			return nullptr;
		}
		if (!this->getDouble(y)) {
			delete returnLine;
			return nullptr;
		}
		if (!this->matchChar(')')) {
			delete returnLine;
			return nullptr;
		}
		returnLine->addNode(new TPoint(x, y));
	}
	if (!this->matchChar(')')) {
		delete returnLine;
		return nullptr;
	}
	return returnLine;
}


bool FileParser::readInLineDisplaySettings(LineDisplaySettings& lds) {
	double lineSize, nodeSize;
	COLORREF lineColor, nodeColor;
	if (!this->matchChar('(')) return false;
	if (!this->getDouble(lineSize)) return false;
	if (!this->matchChar(',')) return false;
	if (!this->getDouble(nodeSize)) return false;
	if (!this->matchChar(',')) return false;
	if (!this->getDWORD(lineColor)) return false;
	if (!this->matchChar(',')) return false;
	if (!this->getDWORD(nodeColor)) return false;
	if (!this->matchChar(')')) return false;
	lds = LineDisplaySettings(lineSize, nodeSize, lineColor, nodeColor);
	return true;
}

bool FileParser::readInBounds(BoundSettings& bs) {
	if (!this->matchChar('(')) return false;
	if (!this->getDouble(bs.leftPosition)) return false;
	if (!this->matchChar(',')) return false;
	if (!this->getDouble(bs.leftValue)) return false;
	if (!this->matchChar(','))  return false;
	if (!this->getDouble(bs.topPosition))  return false;
	if (!this->matchChar(','))  return false;
	if (!this->getDouble(bs.topValue))  return false;
	if (!this->matchChar(','))  return false;
	if (!this->getDouble(bs.rightPosition)) return false;
	if (!this->matchChar(',')) return false;
	if (!this->getDouble(bs.rightValue))  return false;
	if (!this->matchChar(',')) return false;
	if (!this->getDouble(bs.bottomPosition)) return false;
	if (!this->matchChar(','))  return false;
	if (!this->getDouble(bs.bottomValue))  return false;
	if (!this->matchChar(')'))  return false;
	return true;
}


bool FileParser::readInImage(ChartProject* chartProject) {
	string name;
	double x, y, w, h;
	size_t imgDataSize;
	if (!this->matchChar('(')) return false;
	if (!this->getString(name)) return false;
	if (!this->matchChar(',')) return false;
	if (!this->getDouble(x)) return false;
	if (!this->matchChar(',')) return false;
	if (!this->getDouble(y)) return false;
	if (!this->matchChar(',')) return false;
	if (!this->getDouble(w)) return false;
	if (!this->matchChar(',')) return false;
	if (!this->getDouble(h)) return false;
	if (!this->matchChar(',')) return false;
	if (!this->getSize(imgDataSize)) return false;
	//if (!parser.matchChar(':')) break; if we do this itll eat into the image data
	char* imgData = new char[imgDataSize];
	if (imgData) {
		if (!this->getData(imgData, imgDataSize)) {
			delete[] imgData;
			return false;
		}

		char tempPath[MAX_PATH + 1];
		if (GetTempPathA(sizeof(tempPath), tempPath)) {
			char tempFileName[MAX_PATH + 1];
			if (GetTempFileNameA(tempPath, "img", 0, tempFileName)) {
				string tempFileString = tempFileName;
				FILE* imgOut = NULL;
				if (!fopen_s(&imgOut, tempFileString.c_str(), "wb")) {
					fwrite(imgData, imgDataSize, 1, imgOut);
					fclose(imgOut);
					ChartImage* newImage = chartProject->addImage(tempFileString);
					newImage->move(TPoint(x,y), newImage->getSize());
					newImage->name = name;
					remove(tempFileString.c_str());
				} else {
					delete[] imgData;
					this->setError("Unable to generate temp file for image loading");
					return false;
				}
			} else {
				delete[] imgData;
				this->setError("Unable to generate temp file name for image loading");
				return false;
			}
		} else {
			delete[] imgData;
			this->setError("Unable to generate temp path for image loading");
			return false;
		}
		delete[] imgData;
	} else {
		this->setError("Unable to allocate space for image");
		return false;
	}
	if (!this->matchChar(')')) return false;
	return true;
}

bool FileParser::matchChar(int chr) {
	if (look != chr || look==EOF) {
		string msg = "Expected '";
		msg += chr;
		msg += "', but got  '";
		msg += look;
		msg += "'";
		setError(msg);
		return !hasError;
	}
	getChar();
	skipWhite();
	return !hasError;
}


bool FileParser::isNotEOF() {
	return look != EOF;
}

void FileParser::setError(string msg) {
	hasError = true;
	errorString = msg;
}
