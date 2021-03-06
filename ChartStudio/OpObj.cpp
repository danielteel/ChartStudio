#include "stdafx.h"
#include "OpObj.h"

bool isAboutEquals(double x, double y) {
	if (fabs(x - y) < 0.0000001)
		return true; //they are same
	return false; //they are not same
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


// class NullObj extends OpObj {
// 	constructor(){
// 		super(null, OpObjType.null, null, true);
// 	}

// 	getCopy(){
// 		return this;
// 	}
// 	setTo(obj){
// 		throw new Error("tried to write to null");
// 	}

// 	eqaulTo(obj){
// 		if (obj._value !== null) return false;
// 		return true;
// 	}
// 	notEqualTo(obj){
// 		return !this.eqaulTo(obj);
// 	}
// 	smallerThan(obj){
// 		return false;
// 	}
// 	greaterThan(obj){
// 		return false;
// 	}
// 	smallerOrEqualThan(obj){
// 		return false;
// 	}
// 	greaterOrEqualThan(obj){
// 		return false;
// 	}
// }


// class OpObj {
// 	constructor(name="", objType=null, value=null, isConstant=false){
// 		this._name=name;
// 		this._objType=objType;
// 		this._value=value;
// 		this._isConstant=isConstant;
// 	}

// 	get name(){
// 		return this._name;	
// 	}

// 	get objType(){
// 		return this._objType;
// 	}

// 	get isConstant(){
// 		return this._isConstant;
// 	}

// 	get value(){
// 		return this._value;
// 	}
// }

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


// class BoolObj extends OpObj {
// 	constructor(name, initialVal=false, isConstant=false){
// 		super(name, OpObjType.bool, initialVal===null?null:Boolean(initialVal), isConstant);
// 	}

// 	getCopy(){
// 		return new BoolObj(this.name, this._value, this._isConstant);
// 	}

// 	setTo(obj){
// 		if (this._isConstant) return "Tried to write to constant bool";
// 		if (obj instanceof OpObj === false) return "Tried to set bool to invalid type";

// 		let type=obj._objType;
// 		if (type===OpObjType.register) type=obj._curValType;

// 		switch (type){
// 		case OpObjType.null:
// 			this._value=null;
// 			break;
// 		case OpObjType.bool:
// 			this._value=obj._value;
// 			break;
// 		case OpObjType.num:
// 			this._value=obj._value===null ? null : Boolean(obj._value);
// 			break;
// 		default:
// 			return "Tried to set bool to invalid type";
// 		}

// 		return null;
// 	}
// 	eqaulTo(obj){
// 		let type=obj._objType;
// 		if (type===OpObjType.register) type=obj._curValType;

// 		switch (type){
// 		case OpObjType.null:
// 			return this.value===null;
// 		case OpObjType.bool:
// 			return this._value===obj._value;
// 		case OpObjType.num:
// 			return Utils.isAboutEquals(Number(this._value), obj._value);
// 		default:
// 			throw new Error("Tried to do comparison to invalid type");
// 		}
// 	}    
// 	notEqualTo(obj){
// 		return !this.eqaulTo(obj);
// 	}
// 	smallerThan(obj){
// 		let type=obj._objType;
// 		if (type===OpObjType.register) type=obj._curValType;
// 		switch (type){
// 		case OpObjType.bool:
// 			return this._value<obj._value;
// 		case OpObjType.num:
// 			return Number(this._value)<obj._value;
// 		default:
// 			throw new Error("Tried to do comparison to invalid type");
// 		}
// 	}
// 	greaterThan(obj){
// 		let type=obj._objType;
// 		if (type===OpObjType.register) type=obj._curValType;
// 		switch (type){
// 		case OpObjType.bool:
// 			return this._value>obj._value;
// 		case OpObjType.num:
// 			return Number(this._value)>obj._value;
// 		default:
// 			throw new Error("Tried to do comparison to invalid type");
// 		}
// 	}
// 	smallerOrEqualThan(obj){
// 		return this.smallerThan(obj)||this.eqaulTo(obj);
// 	}
// 	greaterOrEqualThan(obj){
// 		return this.greaterThan(obj)||this.eqaulTo(obj);
// 	}
// }


// class StringObj extends OpObj {
// 	constructor(name, initialVal="", isConstant=false){
// 		super(name, OpObjType.string,  initialVal===null?null:String(initialVal), isConstant);
// 	}

// 	getCopy(){
// 		return new StringObj(this.name, this._value, this._isConstant);
// 	}

// 	setTo(obj){
// 		if (this._isConstant) return "Tried to write to constant string";
// 		if (obj instanceof OpObj === false) return "Tried to set string to invalid type";

// 		let type=obj._objType;
// 		if (type===OpObjType.register) type=obj._curValType;

// 		switch (type){
// 			case OpObjType.null:
// 				this._value=null;
// 				break;
// 			case OpObjType.string:
// 				this._value=obj._value;
// 				break;
// 			default:
// 				return "Tried to set string to invalid type";
// 		}
// 		return null;
// 	}

// 	eqaulTo(obj){
// 		let type=obj._objType;
// 		if (type===OpObjType.register) type=obj._curValType;

// 		if (obj._value===null && this._value!==null) return false;
// 		if (this._value===null && obj._value!==null) return false;

// 		switch (type){
// 			case OpObjType.null:
// 				return this._value===null;
// 			case OpObjType.string:
// 				return this._value===obj._value;
// 			default:
// 				throw new Error("Tried to do comparison to invalid type");
// 		}
// 	}
// 	notEqualTo(obj){
// 		return !this.eqaulTo(obj);
// 	}
// 	smallerThan(obj){
// 		throw new Error("Tried to do invalid comparison");
// 	}
// 	greaterThan(obj){
// 		throw new Error("Tried to do invalid comparison");
// 	}
// 	smallerOrEqualThan(obj){
// 		throw new Error("Tried to do invalid comparison");
// 	}
// 	greaterOrEqualThan(obj){
// 		throw new Error("Tried to do invalid comparison");
// 	}
// }

// module.exports={OpObjType, OpObj, NullObj, RegisterObj, StringObj, NumberObj, BoolObj};

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
		if (this->value == nullopt || numObj->value == nullopt) {
			return this->value == numObj->value;
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
		strObj = &static_cast<RegisterObj*>(obj)->getNumberObj();
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

	StringObj* numObj = nullptr;
	if (obj->objType == OpObjType::Register && obj->valueType == OpObjType::String) {
		numObj = &static_cast<RegisterObj*>(obj)->getNumberObj();
	}
	if (obj->objType == OpObjType::String) {
		numObj = static_cast<StringObj*>(obj);
	}
	if (numObj) {
		if (this->value == nullopt || numObj->value == nullopt) {
			return this->value == numObj->value;
		}
		return *this->value==*numObj->value;
	}
	throw "tried to compare object to incompatible object";
}

bool StringObj::notEqualTo(OpObj * obj) {
	return !this->equalTo(obj);
}
