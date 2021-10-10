#pragma once
#include <vector>
#include <string>
#include "UnlinkedObj.h"
#include "OpObj.h"

using namespace std;
class ExternalDef {
public:
	string name;
	IdentityType type;
	OpObj* opObj;
	OpObj*(*functionAddress) (OpObj*(*)());
	vector<IdentityType> params;
	IdentityType returnType;

	ExternalDef(string name, IdentityType type, OpObj* opObj);
	ExternalDef(string name, IdentityType returnType, vector<IdentityType> params, OpObj*(*fn) (OpObj*(*)()));
	ExternalDef();
};