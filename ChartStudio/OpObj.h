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
	OpObj(OpObjType type, OpObjType valueType, bool isConstant);
	OpObj();
	virtual ~OpObj();

	OpObjType objType;
	OpObjType valueType;
	bool isConstant;

	virtual void setTo(OpObj* obj) = 0;

	virtual bool equalTo(OpObj * obj) = 0;
	virtual bool notEqualTo(OpObj * obj) = 0;
	virtual bool greaterThan(OpObj * obj) = 0;
	virtual bool greaterOrEqualsThan(OpObj * obj) = 0;
	virtual bool smallerThan(OpObj * obj) = 0;
	virtual bool smallerOrEqualsThan(OpObj * obj) = 0;
};

class StringObj :public OpObj {
public:
	StringObj(optional<string> initialValue, bool isConstant);
	virtual ~StringObj();

	optional<string> value;

	void setTo(OpObj* obj);
	bool equalTo(OpObj* obj);
	bool notEqualTo(OpObj* obj);
};

class NumberObj : public OpObj{
public:
	NumberObj(optional<double> initialValue, bool isConstant);
	virtual ~NumberObj();

	optional<double> value;

	void setTo(OpObj* obj);
	bool equalTo(OpObj * obj);
	bool notEqualTo(OpObj * obj);
	bool greaterThan(OpObj * obj);
	bool greaterOrEqualsThan(OpObj * obj);
	bool smallerThan(OpObj * obj);
	bool smallerOrEqualsThan(OpObj * obj);
};