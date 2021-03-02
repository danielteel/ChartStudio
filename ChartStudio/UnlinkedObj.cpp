#include "stdafx.h"
#include "UnlinkedObj.h"



UnlinkedObj::UnlinkedObj(RegisterId registerId) {
	this->unlinkedType = UnlinkedType::Register;
	this->registerId = registerId;
}

UnlinkedObj::UnlinkedObj(IdentityType type, size_t scope, size_t index) {
	this->unlinkedType = UnlinkedType::Variable;
	this->type = type;
	this->scope = scope;
	this->index = index;
}

UnlinkedObj::UnlinkedObj(string value) {
	this->unlinkedType = UnlinkedType::Literal;
	this->type = IdentityType::String;
	this->sValue = value;
}

UnlinkedObj::UnlinkedObj(double value) {
	this->unlinkedType = UnlinkedType::Literal;
	this->type = IdentityType::Double;
	this->dValue = value;
}

UnlinkedObj::UnlinkedObj(bool value) {
	this->unlinkedType = UnlinkedType::Literal;
	this->type = IdentityType::Bool;
	this->bValue = value;
}

UnlinkedObj::UnlinkedObj() {
	this->unlinkedType = UnlinkedType::Null;
}