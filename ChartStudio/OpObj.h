#pragma once
#include <string>
#include <optional>

using namespace std;

enum class OpObjType {
	Null,
	Register,
	Bool,
	Number,
	String,
};

class OpObj {
public:
	OpObj(OpObjType type, bool isConstant);
	OpObj();
	virtual ~OpObj();

	OpObjType objType;
	bool isConstant;

	virtual void setTo(OpObj* obj) = 0;
};

class NumberObj : public OpObj{
public:
	NumberObj(optional<double> initialValue, bool isConstant);
	virtual ~NumberObj();

	optional<double> value;

	void setTo(OpObj* obj);
};