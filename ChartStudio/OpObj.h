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
};

class NullObj : public OpObj {
	public:
		NullObj();

		NullObj getCopy();
		void setTo(OpObj obj);

		virtual ~NullObj() {}
};

class BoolObj : public OpObj {
	public:
		virtual ~BoolObj() {}
		BoolObj(optional<bool> initialValue, bool isConstant) : OpObj(OpObjType::Bool, isConstant) {
			this->value = initialValue;
		}

		optional<bool> value;
};

class NumberObj : public OpObj {
	public:
		virtual ~NumberObj() {}
		NumberObj(optional<double> initialValue, bool isConstant) : OpObj(OpObjType::Number, isConstant) {
			this->value = initialValue;
		}

		optional<double> value;
};

class StringObj : public OpObj {
	public:
		virtual ~StringObj() {}
		StringObj(optional<string> initialValue, bool isConstant) : OpObj(OpObjType::String, isConstant) {
			this->value = initialValue;
		}

		optional<string> value;
};


class RegisterObj : public OpObj {
public:

	virtual ~RegisterObj() {}
	RegisterObj() : OpObj(OpObjType::Register, false) {

	}

	NullObj nullObj = NullObj();
	BoolObj boolObj = BoolObj(nullopt, false);
	NumberObj numberObj = NumberObj(nullopt, false);
	StringObj stringObj = StringObj(nullopt, false);
};