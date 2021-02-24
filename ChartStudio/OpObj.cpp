#include "stdafx.h"
#include "OpObj.h"


OpObj::OpObj() {
}


OpObj::OpObj(string name, OpObjType type, bool isConstant) {
	this->name = name;
	this->objType = type;
	this->isConstant = isConstant;
}

OpObj::~OpObj() {
}
