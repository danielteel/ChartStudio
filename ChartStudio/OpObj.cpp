#include "stdafx.h"
#include "OpObj.h"

bool isAboutEquals(double x, double y) {
	if (fabs(x - y) < 0.0000001f)
		return true;
	return false;
}

OpObj::OpObj(OpObjType type, OpObjType valueType, bool isConstant) {
	this->objType = type;
	this->valueType = valueType;
	this->isConstant = isConstant;
}

OpObj::OpObj() {
}


OpObj::~OpObj() {
}


NullObj::NullObj() : OpObj(OpObjType::Null, OpObjType::Null, true) {
}

NullObj::~NullObj() {
}

void NullObj::setTo(OpObj * obj) {
	throw "tried to write to constant null obj";
}

bool NullObj::equalTo(OpObj* obj) {
	if (!obj) throw "tried to compare object to null pointer";
	switch (obj->valueType) {
	case OpObjType::Null:
		return true;
	case OpObjType::Bool:
		if (obj->objType == OpObjType::Register) {
			if (static_cast<RegisterObj*>(obj)->boolObj.value == nullopt) return true;
		} else {
			if (static_cast<BoolObj*>(obj)->value == nullopt) return true;
		}
		break;
	case OpObjType::Number:
		if (obj->objType == OpObjType::Register) {
			if (static_cast<RegisterObj*>(obj)->numObj.value == nullopt) return true;
		} else {
			if (static_cast<NumberObj*>(obj)->value == nullopt) return true;
		}
		break;
	case OpObjType::String:
		if (obj->objType == OpObjType::Register) {
			if (static_cast<RegisterObj*>(obj)->stringObj.value == nullopt) return true;
		} else {
			if (static_cast<StringObj*>(obj)->value == nullopt) return true;
		}
		break;
	}
	return false;
}

bool NullObj::notEqualTo(OpObj* obj) {
	return !this->equalTo(obj);
}


bool NullObj::greaterThan(OpObj * obj) {
	throw "attempted greaterThan comparison on null object";
}

bool NullObj::greaterOrEqualsThan(OpObj * obj) {
	throw "attempted greaterOrEqualsThan comparison on null object";
}

bool NullObj::smallerThan(OpObj * obj) {
	throw "attempted lessThan comparison on null object";
}

bool NullObj::smallerOrEqualsThan(OpObj * obj) {
	throw "attempted lessOrEqualsThan comparison on null object";
}




NumberObj::NumberObj() : OpObj(OpObjType::Number, OpObjType::Number, false) {
	this->value = nullopt;
}
NumberObj::NumberObj(optional<double> initialValue, bool isConstant) : OpObj(OpObjType::Number, OpObjType::Number, isConstant) {
	this->value = initialValue;
}

NumberObj::~NumberObj() {
}

void NumberObj::setTo(OpObj * obj) {
	if (isConstant) throw "tried to write to constant";
	if (!obj) throw "tried to setTo with null pointer";
	if (obj->objType == OpObjType::Null || (obj->objType == OpObjType::Register && obj->valueType == OpObjType::Null)) {
		this->value = nullopt;
		return;
	}

	NumberObj* numObj = nullptr;
	if (obj->objType == OpObjType::Register && obj->valueType==OpObjType::Number) {
		numObj = &static_cast<RegisterObj*>(obj)->numObj;
	}
	if (obj->objType == OpObjType::Number) {
		numObj = static_cast<NumberObj*>(obj);
	}
	if (numObj) {
		this->value = numObj->value;
	}
	throw "tried to set object to incompatible object";
}

bool NumberObj::equalTo(OpObj* obj) {
	if (!obj) throw "tried to compare object to null pointer";
	if (obj->objType == OpObjType::Null || (obj->objType == OpObjType::Register && obj->valueType == OpObjType::Null)) {
		if (this->value == nullopt) {
			return true;
		}
		return false;
	}

	NumberObj* numObj = nullptr;
	if (obj->objType == OpObjType::Register && obj->valueType == OpObjType::Number) {
		numObj = &static_cast<RegisterObj*>(obj)->numObj;
	}
	if (obj->objType == OpObjType::Number) {
		numObj = static_cast<NumberObj*>(obj);
	}
	if (numObj) {
		if ((this->value == nullopt && numObj->value == nullopt)) {
			return true;
		} else if (this->value == nullopt || numObj->value == nullopt) {
			return false;
		}
		return isAboutEquals(*this->value, *numObj->value);
	}
	throw "tried to compare object to incompatible object";
}

bool NumberObj::notEqualTo(OpObj* obj) {
	return !this->equalTo(obj);
}

bool NumberObj::greaterThan(OpObj* obj) {
	if (!obj) throw "tried to compare object to null pointer";

	NumberObj* numObj = nullptr;
	if (obj->objType == OpObjType::Register && obj->valueType == OpObjType::Number) {
		numObj = &static_cast<RegisterObj*>(obj)->numObj;
	}
	if (obj->objType == OpObjType::Number) {
		numObj = static_cast<NumberObj*>(obj);
	}
	if (numObj) {
		if (this->value == nullopt || numObj->value == nullopt) {
			throw "tried to do size comparison with null";
		}
		return *this->value > *numObj->value;
	}
	throw "tried to compare object to incompatible object";
}

bool NumberObj::greaterOrEqualsThan(OpObj* obj) {
	return this->greaterThan(obj) || this->equalTo(obj);
}

bool NumberObj::smallerThan(OpObj* obj) {
	return !this->greaterOrEqualsThan(obj);
}

bool NumberObj::smallerOrEqualsThan(OpObj* obj) {
	return !this->greaterThan(obj);
}

StringObj::StringObj() : OpObj(OpObjType::String, OpObjType::String, false) {
	this->value = nullopt;
}
StringObj::StringObj(optional<string> initialValue, bool isConstant) : OpObj(OpObjType::String, OpObjType::String, isConstant) {
	this->value = initialValue;
}

StringObj::~StringObj() {
}

void StringObj::setTo(OpObj * obj) {
	if (isConstant) throw "tried to write to constant";
	if (!obj) throw "tried to setTo with null pointer";
	if (obj->objType == OpObjType::Null || (obj->objType == OpObjType::Register && obj->valueType == OpObjType::Null)) {
		this->value = nullopt;
		return;
	}

	StringObj* strObj = nullptr;
	if (obj->objType == OpObjType::Register && obj->valueType == OpObjType::String) {
		strObj = &static_cast<RegisterObj*>(obj)->stringObj;
	}
	if (obj->objType == OpObjType::String) {
		strObj = static_cast<StringObj*>(obj);
	}
	if (strObj) {
		this->value = strObj->value;
	}
	throw "tried to set object to incompatible object";
}

bool StringObj::equalTo(OpObj * obj) {
	if (!obj) throw "tried to compare object to null pointer";
	if (obj->objType == OpObjType::Null || (obj->objType == OpObjType::Register && obj->valueType == OpObjType::Null)) {
		if (this->value == nullopt) {
			return true;
		}
		return false;
	}

	StringObj* strObj = nullptr;
	if (obj->objType == OpObjType::Register && obj->valueType == OpObjType::String) {
		strObj = &static_cast<RegisterObj*>(obj)->stringObj;
	}
	if (obj->objType == OpObjType::String) {
		strObj = static_cast<StringObj*>(obj);
	}
	if (strObj) {
		if (this->value == nullopt && strObj->value == nullopt) {
			return true;
		} else if (this->value == nullopt || strObj->value == nullopt) {
			return false;
		}
		return *this->value == *strObj->value;
	}
	throw "tried to compare object to incompatible object";
}

bool StringObj::notEqualTo(OpObj * obj) {
	return !this->equalTo(obj);
}

bool StringObj::greaterThan(OpObj * obj) {
	throw "attempted greaterThan comparison on string object";
}

bool StringObj::greaterOrEqualsThan(OpObj * obj) {
	throw "attempted greaterOrEqualsThan comparison on string object";
}

bool StringObj::smallerThan(OpObj * obj) {
	throw "attempted lessThan comparison on string object";
}

bool StringObj::smallerOrEqualsThan(OpObj * obj) {
	throw "attempted lessOrEqualsThan comparison on string object";
}


BoolObj::BoolObj() : OpObj(OpObjType::Bool, OpObjType::Bool, false) {
	this->value = nullopt;
}

BoolObj::BoolObj(optional<bool> initialValue, bool isConstant) : OpObj(OpObjType::Bool, OpObjType::Bool, isConstant) {
	this->value = initialValue;
}

BoolObj::~BoolObj() {
}

void BoolObj::setTo(OpObj * obj) {
	if (isConstant) throw "tried to write to constant";
	if (!obj) throw "tried to setTo with null pointer";
	if (obj->objType == OpObjType::Null || (obj->objType == OpObjType::Register && obj->valueType == OpObjType::Null)) {
		this->value = nullopt;
		return;
	}

	BoolObj* boolObj = nullptr;
	if (obj->objType == OpObjType::Register && obj->valueType == OpObjType::Bool) {
		boolObj = &static_cast<RegisterObj*>(obj)->boolObj;
	}
	if (obj->objType == OpObjType::Bool) {
		boolObj = static_cast<BoolObj*>(obj);
	}
	if (boolObj) {
		this->value = boolObj->value;
	}
	throw "tried to set object to incompatible object";
}

bool BoolObj::equalTo(OpObj* obj) {
	if (!obj) throw "tried to compare object to null pointer";
	if (obj->objType == OpObjType::Null || (obj->objType == OpObjType::Register && obj->valueType == OpObjType::Null)) {
		if (this->value == nullopt) {
			return true;
		}
		return false;
	}

	BoolObj* boolObj = nullptr;
	if (obj->objType == OpObjType::Register && obj->valueType == OpObjType::Bool) {
		boolObj = &static_cast<RegisterObj*>(obj)->boolObj;
	}
	if (obj->objType == OpObjType::Bool) {
		boolObj = static_cast<BoolObj*>(obj);
	}
	if (boolObj) {
		if ((this->value == nullopt && boolObj->value == nullopt)) {
			return true;
		} else if (this->value == nullopt || boolObj->value == nullopt) {
			return false;
		}
		return *this->value == *boolObj->value;
	}
	throw "tried to compare object to incompatible object";
}

bool BoolObj::notEqualTo(OpObj* obj) {
	return !this->equalTo(obj);
}

bool BoolObj::greaterThan(OpObj * obj) {
	throw "attempted greaterThan comparison on bool object";
}

bool BoolObj::greaterOrEqualsThan(OpObj * obj) {
	throw "attempted greaterOrEqualsThan comparison on bool object";
}

bool BoolObj::smallerThan(OpObj * obj) {
	throw "attempted lessThan comparison on bool object";
}

bool BoolObj::smallerOrEqualsThan(OpObj * obj) {
	throw "attempted lessOrEqualsThan comparison on bool object";
}


RegisterObj::RegisterObj() {
}

RegisterObj::~RegisterObj() {
}

void RegisterObj::setTo(OpObj * obj) {
	if (!obj) throw "tried to setTo with null pointer";

	this->valueType = obj->valueType;

	if (obj->objType == OpObjType::Register) {
		RegisterObj* regObj = static_cast<RegisterObj*>(obj);
		this->stringObj = regObj->stringObj;
		this->boolObj = regObj->boolObj;
		this->numObj = regObj->numObj;
		this->nullObj = regObj->nullObj;
	} else if (obj->objType==OpObjType::Null){
		this->nullObj = *static_cast<NullObj*>(obj);
	} else if (obj->objType == OpObjType::Bool) {
		this->boolObj = *static_cast<BoolObj*>(obj);
	} else if (obj->objType == OpObjType::String) {
		this->stringObj = *static_cast<StringObj*>(obj);
	} else if (obj->objType == OpObjType::Number) {
		this->numObj = *static_cast<NumberObj*>(obj);
	} else {
		throw "tried to set register to unknown data type";
	}
}


OpObj* RegisterObj::getNativeObj() {
	switch (this->valueType) {
	case OpObjType::Null:
		return &this->nullObj;
	case OpObjType::Bool:
		return &this->boolObj;
	case OpObjType::String:
		return &this->stringObj;
	case OpObjType::Number:
		return &this->numObj;
	default:
		throw "register has unknown value type";
	}
}

bool RegisterObj::equalTo(OpObj * obj) {
	return this->getNativeObj()->equalTo(obj);
}

bool RegisterObj::notEqualTo(OpObj * obj) {
	return this->getNativeObj()->notEqualTo(obj);
}

bool RegisterObj::greaterThan(OpObj * obj) {
	return this->getNativeObj()->greaterThan(obj);
}

bool RegisterObj::greaterOrEqualsThan(OpObj * obj) {
	return this->getNativeObj()->greaterOrEqualsThan(obj);
}

bool RegisterObj::smallerThan(OpObj * obj) {
	return this->getNativeObj()->smallerThan(obj);
}

bool RegisterObj::smallerOrEqualsThan(OpObj * obj) {
	return this->getNativeObj()->smallerOrEqualsThan(obj);
}
