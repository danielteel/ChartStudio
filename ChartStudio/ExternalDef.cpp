#include "stdafx.h"
#include "ExternalDef.h"

ExternalDef::ExternalDef(string name, IdentityType type, OpObj* opObj) {
	this->name = name;
	this->type = type;
	this->opObj = opObj;
	this->functionAddress = nullptr;
}
ExternalDef::ExternalDef(string name, IdentityType returnType, vector<IdentityType> params, OpObj*(*fn) (OpObj*(*)())) {
	this->name = name;
	this->type = IdentityType::Function;
	this->returnType = returnType;
	this->params = params;
	this->functionAddress = fn;
	this->opObj = nullptr;
}	
ExternalDef::ExternalDef() {
}