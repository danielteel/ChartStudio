#pragma once
#include <string>
#include <vector>
#include "OpObj.h"
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

	void link();

	string errorMsg;
	CodeState codeState=CodeState::building;
	
	size_t debugCodeLine;

	vector<OpCode> code;

	RegisterObj eax;
	RegisterObj ebx;
	RegisterObj ecx;
	BoolObj trueObj = BoolObj(true, true);
	BoolObj falseObj = BoolObj(false, true);
	NumberObj zeroObj = NumberObj(0, true);
	NullObj nullObj = NullObj();
};



