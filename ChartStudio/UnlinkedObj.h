#pragma once
#include <string>

using namespace std;

enum class IdentityType {
	Null,
	Bool,
	Double,
	String,
	Function
};

enum class UnlinkedType {
	Register,
	Variable,
	Literal,
	Null
};

enum class RegisterId {
	eax,
	ebx,
	ecx
};

class UnlinkedObj {
public:
	UnlinkedType unlinkedType;
	RegisterId registerId;
	IdentityType type;
	double dValue;
	string sValue;
	bool bValue;
	size_t scope;
	size_t index;

	UnlinkedObj(RegisterId registerId);
	UnlinkedObj(IdentityType type, size_t scope, size_t index);
	UnlinkedObj(string value);
	UnlinkedObj(double value);
	UnlinkedObj(bool value);
	UnlinkedObj();
};