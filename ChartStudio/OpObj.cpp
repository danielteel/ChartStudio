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
			if (static_cast<RegisterObj*>(obj)->getBoolObj().value == nullopt) return true;
		} else {
			if (static_cast<BoolObj*>(obj)->value == nullopt) return true;
		}
		break;
	case OpObjType::Number:
		if (obj->objType == OpObjType::Register) {
			if (static_cast<RegisterObj*>(obj)->getNumberObj().value == nullopt) return true;
		} else {
			if (static_cast<NumberObj*>(obj)->value == nullopt) return true;
		}
		break;
	case OpObjType::String:
		if (obj->objType == OpObjType::Register) {
			if (static_cast<RegisterObj*>(obj)->getStringObj().value == nullopt) return true;
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

// class RegisterObj extends OpObj {
// 	constructor(name){
// 		super(name, OpObjType.register, null, false);
// 		this._curValType=OpObjType.num;
// 		this.stringObj=new StringObj(null, null, false);
// 		this.boolObj=new BoolObj(null, null, false);
// 		this.numberObj=new NumberObj(null, null, false);
// 		this.nullObj = new NullObj();
// 	}

// 	getCopy(asNative){
// 		if (asNative){
// 			return this.getNativeObj();
// 		}
// 		let newObj=new RegisterObj(this.name+"Copy");
// 		newObj._curValType=this._curValType;
// 		newObj._value=this._value;
// 		newObj._isConstant=this._isConstant;
// 		return newObj;
// 	}

// 	setTo(obj){
// 		if (obj instanceof OpObj === false) return "Tried to set register to invalid type";

// 		if (obj._objType===OpObjType.register){
// 			this._curValType=obj._curValType;
// 		}else{
// 			this._curValType=obj._objType;
// 		}
// 		this._value=obj._value;
// 		return null;
// 	}

// 	getNativeObj(){
// 		switch (this._curValType){
// 		case OpObjType.null:
// 			return this.nullObj;
// 		case OpObjType.string:
// 			this.stringObj._value=this._value;
// 			return this.stringObj;
// 		case OpObjType.bool:
// 			this.boolObj._value=this._value;
// 			return this.boolObj;
// 		case OpObjType.num:
// 			this.numberObj._value=this._value;
// 			return this.numberObj;
// 		}
// 	}

// 	eqaulTo(obj){
// 		return this.getNativeObj().eqaulTo(obj);
// 	}
// 	notEqualTo(obj){
// 		return this.getNativeObj().notEqualTo(obj);
// 	}
// 	smallerThan(obj){
// 		return this.getNativeObj().smallerThan(obj);
// 	}
// 	greaterThan(obj){
// 		return this.getNativeObj().greaterThan(obj);
// 	}
// 	smallerOrEqualThan(obj){
// 		return this.getNativeObj().smallerOrEqualThan(obj);
// 	}
// 	greaterOrEqualThan(obj){
// 		return this.getNativeObj().greaterOrEqualThan(obj);
// 	}
// }



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
		numObj = &static_cast<RegisterObj*>(obj)->getNumberObj();
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
		numObj = &static_cast<RegisterObj*>(obj)->getNumberObj();
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
		numObj = &static_cast<RegisterObj*>(obj)->getNumberObj();
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
		strObj = &static_cast<RegisterObj*>(obj)->getStringObj();
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
		strObj = &static_cast<RegisterObj*>(obj)->getStringObj();
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
		boolObj = &static_cast<RegisterObj*>(obj)->getBoolObj();
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
		boolObj = &static_cast<RegisterObj*>(obj)->getBoolObj();
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
	}
}
