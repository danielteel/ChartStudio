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
	Function
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
	virtual OpObj* getCopy() = 0;

	virtual bool equalTo(OpObj * obj) = 0;
	virtual bool notEqualTo(OpObj * obj) = 0;
	virtual bool greaterThan(OpObj * obj) = 0;
	virtual bool greaterOrEqualsThan(OpObj * obj) = 0;
	virtual bool smallerThan(OpObj * obj) = 0;
	virtual bool smallerOrEqualsThan(OpObj * obj) = 0;
};

class FuncObj : public OpObj {
public:
	FuncObj();
	FuncObj(OpObj*(*fn) (OpObj*(*fn) ()));
	virtual ~FuncObj();

	OpObj*(*fn) (OpObj*(*fn) ()) = nullptr;
	void setTo(OpObj* obj);

	OpObj* getCopy();

	bool equalTo(OpObj* obj);
	bool notEqualTo(OpObj* obj);
	bool greaterThan(OpObj * obj);
	bool greaterOrEqualsThan(OpObj * obj);
	bool smallerThan(OpObj * obj);
	bool smallerOrEqualsThan(OpObj * obj);
};

class StringObj : public OpObj {
public:
	StringObj();
	StringObj(optional<string> initialValue, bool isConstant);
	virtual ~StringObj();

	optional<string> value;

	void setTo(OpObj* obj);
	OpObj* getCopy();

	bool equalTo(OpObj* obj);
	bool notEqualTo(OpObj* obj);
	bool greaterThan(OpObj * obj);
	bool greaterOrEqualsThan(OpObj * obj);
	bool smallerThan(OpObj * obj);
	bool smallerOrEqualsThan(OpObj * obj);
};

class NumberObj : public OpObj {
public:
	NumberObj();
	NumberObj(optional<double> initialValue, bool isConstant);
	virtual ~NumberObj();

	optional<double> value;

	void setTo(OpObj* obj);
	OpObj* getCopy();

	bool equalTo(OpObj * obj);
	bool notEqualTo(OpObj * obj);
	bool greaterThan(OpObj * obj);
	bool greaterOrEqualsThan(OpObj * obj);
	bool smallerThan(OpObj * obj);
	bool smallerOrEqualsThan(OpObj * obj);
};

class BoolObj : public OpObj {
public:
	BoolObj();
	BoolObj(optional<bool> initialValue, bool isConstant);
	virtual ~BoolObj();

	optional<bool> value;

	void setTo(OpObj* obj);
	OpObj* getCopy();

	bool equalTo(OpObj * obj);
	bool notEqualTo(OpObj * obj);
	bool greaterThan(OpObj * obj);
	bool greaterOrEqualsThan(OpObj * obj);
	bool smallerThan(OpObj * obj);
	bool smallerOrEqualsThan(OpObj * obj);
};

class NullObj : public OpObj {
public:
	NullObj();
	virtual ~NullObj();

	void setTo(OpObj* obj);
	OpObj* getCopy();

	bool equalTo(OpObj * obj);
	bool notEqualTo(OpObj * obj);
	bool greaterThan(OpObj * obj);
	bool greaterOrEqualsThan(OpObj * obj);
	bool smallerThan(OpObj * obj);
	bool smallerOrEqualsThan(OpObj * obj);
};

class RegisterObj : public OpObj {
public:
	RegisterObj();
	virtual ~RegisterObj();

	BoolObj boolObj;
	StringObj stringObj;
	NumberObj numObj;
	NullObj nullObj;

	void setTo(OpObj* obj);
	OpObj* getCopy();

	OpObj* getNativeObj();
	bool equalTo(OpObj * obj);
	bool notEqualTo(OpObj * obj);
	bool greaterThan(OpObj * obj);
	bool greaterOrEqualsThan(OpObj * obj);
	bool smallerThan(OpObj * obj);
	bool smallerOrEqualsThan(OpObj * obj);
};