#pragma once
#include <string>
#include <vector>
#include "OpCode.h"

enum class CodeState {
	building,
	optimized,
	ready
};


class Program {
public:
	Program();
	~Program();

	void reset();

	size_t addCode(OpCode op);
	void insertCode(OpCode op, size_t afterThis);
	void insertCode(vector<OpCode> opCodes, size_t afterThis);

	string errorMsg;
	CodeState codeState;
	
	size_t debugCodeLine;

	vector<OpCode> code;
};



