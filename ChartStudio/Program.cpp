#include "stdafx.h"
#include "Program.h"
#include "OpObj.h"
#include "OpCode.h"

Program::Program() {
	this->errorMsg = "";

	this->code.clear();
	this->codeState = CodeState::building;

	this->debugCodeLine = 1;

}


Program::~Program() {
}

void Program::reset() {
	this->errorMsg = "";
	this->code.clear();
	this->codeState = CodeState::building;
	this->debugCodeLine = 1;
}

size_t Program::addCode(OpCode op) {
	this->code.push_back(op);
	return this->code.size();
}

void Program::insertCode(OpCode op, size_t afterThis) {
	vector<OpCode>::iterator whereTo = this->code.begin() + afterThis;
	this->code.insert(whereTo, op);
}

void Program::insertCode(vector<OpCode> opCodes, size_t afterThis) {
	if (opCodes.size() == 0) return;
	for (size_t i = opCodes.size(); i > 0; i--) {
		this->insertCode(opCodes[i-1], afterThis);
	}
}