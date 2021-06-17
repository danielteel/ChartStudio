#pragma once
#include <string>
#include <vector>
#include "OpObj.h"
#include "OpCode.h"
#include "ExternalDef.h"
using namespace std;

enum class CodeState {
	building,
	ready
};


class Program {
public:
	Program();
	~Program();
	void reset();

	void throwError(string message);

	size_t addCode(OpCode op);
	void insertCode(OpCode op, size_t afterThis);
	void insertCode(vector<OpCode> opCodes, size_t afterThis);

	string errorMsg;
	CodeState codeState = CodeState::building;

	size_t debugCodeLine;

	vector<OpCode> code;

	RegisterObj eax;
	RegisterObj ebx;
	RegisterObj ecx;
	BoolObj trueObj = BoolObj(true, true);
	BoolObj falseObj = BoolObj(false, true);
	NumberObj zeroObj = NumberObj(0, true);
	NullObj nullObj = NullObj();

	
	void link();
	OpObj * linkedObject(UnlinkedObj * obj, vector<vector<vector<OpObj*>>>& scopes, bool * needToFree);

	OpObj* execute(vector<ExternalDef> externals);
};



