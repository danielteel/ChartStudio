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
	OpObj(string name, OpObjType type, bool isConstant);
	OpObj();
	virtual ~OpObj();

	string name;
	OpObjType objType;
	bool isConstant;

};

class NullObj : public OpObj {
	public:
		NullObj() : OpObj("", OpObjType::Null, true) {
		}

		virtual ~NullObj() {}
};

class BoolObj : public OpObj {
	public:
		virtual ~BoolObj() {}
		BoolObj(string name, optional<bool> initialValue, bool isConstant) : OpObj(name, OpObjType::Bool, isConstant) {
			this->value = initialValue;
		}

		optional<bool> value;
};

class NumberObj : public OpObj {
	public:
		virtual ~NumberObj() {}
		NumberObj(string name, optional<double> initialValue, bool isConstant) : OpObj(name, OpObjType::Number, isConstant) {
			this->value = initialValue;
		}

		optional<double> value;
};

class StringObj : public OpObj {
	public:
		virtual ~StringObj() {}
		StringObj(string name, optional<string> initialValue, bool isConstant) : OpObj(name, OpObjType::String, isConstant) {
			this->value = initialValue;
		}

		optional<string> value;
};


class RegisterObj : public OpObj {
public:

	virtual ~RegisterObj() {}
	RegisterObj() : OpObj("", OpObjType::Register, false) {

	}

	NullObj nullObj = NullObj();
	BoolObj boolObj = BoolObj("", nullopt, false);
	NumberObj numberObj = NumberObj("", nullopt, false);
	StringObj stringObj = StringObj("", nullopt, false);
};