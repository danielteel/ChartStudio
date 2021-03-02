#include "stdafx.h"
#include "OpCode.h"

OpCode::OpCode(OpCodeType type) {
	this->type = type;
}

OpCode OpCode::ret() {
	return OpCode(OpCodeType::ret);
}
OpCode OpCode::id_Op(OpCodeType type, size_t id) {
	OpCode a(type);
	a.id = id;
	return a;
}
OpCode OpCode::label(size_t id) {
	return OpCode::id_Op(OpCodeType::label, id);
}
OpCode OpCode::call(size_t id) {
	return OpCode::id_Op(OpCodeType::call, id);
}
OpCode OpCode::exCall(size_t id) {
	return OpCode::id_Op(OpCodeType::excall, id);
}
OpCode OpCode::jmp(size_t id) {
	return OpCode::id_Op(OpCodeType::jmp, id);
}
OpCode OpCode::je(size_t id) {
	return OpCode::id_Op(OpCodeType::je, id);
}
OpCode OpCode::jne(size_t id) {
	return OpCode::id_Op(OpCodeType::jne, id);
}
OpCode OpCode::scopeDepth(size_t size) {
	OpCode a(OpCodeType::scopeDepth);
	a.size = size;
	return a;
}

OpCode OpCode::obj0_Op(OpCodeType type, UnlinkedObj obj0) {
	OpCode a(type);
	a.obj0 = obj0;
	return a;
}

OpCode OpCode::test(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::test, obj0);
}
OpCode OpCode::se(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::se, obj0);
}
OpCode OpCode::sne(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::sne, obj0);
}
OpCode OpCode::sa(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::sa, obj0);
}
OpCode OpCode::sae(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::sae, obj0);
}
OpCode OpCode::sb(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::sb, obj0);
}
OpCode OpCode::sbe(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::sbe, obj0);
}
OpCode OpCode::exit(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::exit, obj0);
}
OpCode OpCode::ceil(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::ceil, obj0);
}
OpCode OpCode::floor(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::floor, obj0);
}
OpCode OpCode::abs(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::abs, obj0);
}
OpCode OpCode::toDouble(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::todouble, obj0);
}
OpCode OpCode::toBool(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::tobool, obj0);
}
OpCode OpCode::len(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::len, obj0);
}
OpCode OpCode::lcase(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::lcase, obj0);
}
OpCode OpCode::ucase(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::ucase, obj0);
}
OpCode OpCode::trim(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::trim, obj0);
}
OpCode OpCode::allocDouble(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::allocDouble, obj0);
}
OpCode OpCode::allocBool(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::allocBool, obj0);
}
OpCode OpCode::allocString(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::allocString, obj0);
}
OpCode OpCode::push(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::push, obj0);
}
OpCode OpCode::pop(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::pop, obj0);
}
OpCode OpCode::notOp(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::notOp, obj0);
}
OpCode OpCode::neg(UnlinkedObj obj0) {
	return OpCode::obj0_Op(OpCodeType::neg, obj0);
}
OpCode OpCode::codeLine(string code) {
	OpCode a(OpCodeType::codeLine);
	a.code = code;
	return a;
}
OpCode OpCode::popScope(size_t scope) {
	OpCode a(OpCodeType::popScope);
	a.scope = scope;
	return a;
}

OpCode OpCode::obj0_and_obj1_Op(OpCodeType type, UnlinkedObj obj0, UnlinkedObj obj1) {
	OpCode a(type);
	a.obj0 = obj0;
	a.obj1 = obj1;
	return a;
}

OpCode OpCode::toString(UnlinkedObj obj0, UnlinkedObj obj1) {
	return OpCode::obj0_and_obj1_Op(OpCodeType::tostring, obj0, obj1);
}
OpCode OpCode::cmp(UnlinkedObj obj0, UnlinkedObj obj1) {
	return OpCode::obj0_and_obj1_Op(OpCodeType::cmp, obj0, obj1);
}
OpCode OpCode::concat(UnlinkedObj obj0, UnlinkedObj obj1) {
	return OpCode::obj0_and_obj1_Op(OpCodeType::concat, obj0, obj1);
}
OpCode OpCode::minOp(UnlinkedObj obj0, UnlinkedObj obj1) {
	return OpCode::obj0_and_obj1_Op(OpCodeType::min, obj0, obj1);
}
OpCode OpCode::maxOp(UnlinkedObj obj0, UnlinkedObj obj1) {
	return OpCode::obj0_and_obj1_Op(OpCodeType::max, obj0, obj1);
}
OpCode OpCode::mov(UnlinkedObj obj0, UnlinkedObj obj1) {
	return OpCode::obj0_and_obj1_Op(OpCodeType::mov, obj0, obj1);
}
OpCode OpCode::andOp(UnlinkedObj obj0, UnlinkedObj obj1) {
	return OpCode::obj0_and_obj1_Op(OpCodeType::andOp, obj0, obj1);
}
OpCode OpCode::orOp(UnlinkedObj obj0, UnlinkedObj obj1) {
	return OpCode::obj0_and_obj1_Op(OpCodeType::orOp, obj0, obj1);
}
OpCode OpCode::add(UnlinkedObj obj0, UnlinkedObj obj1) {
	return OpCode::obj0_and_obj1_Op(OpCodeType::add, obj0, obj1);
}
OpCode OpCode::sub(UnlinkedObj obj0, UnlinkedObj obj1) {
	return OpCode::obj0_and_obj1_Op(OpCodeType::sub, obj0, obj1);
}
OpCode OpCode::mul(UnlinkedObj obj0, UnlinkedObj obj1) {
	return OpCode::obj0_and_obj1_Op(OpCodeType::mul, obj0, obj1);
}
OpCode OpCode::div(UnlinkedObj obj0, UnlinkedObj obj1) {
	return OpCode::obj0_and_obj1_Op(OpCodeType::div, obj0, obj1);
}
OpCode OpCode::mod(UnlinkedObj obj0, UnlinkedObj obj1) {
	return OpCode::obj0_and_obj1_Op(OpCodeType::mod, obj0, obj1);
}
OpCode OpCode::exponent(UnlinkedObj obj0, UnlinkedObj obj1) {
	return OpCode::obj0_and_obj1_Op(OpCodeType::exponent, obj0, obj1);
}
OpCode OpCode::pushScope(size_t scope, size_t size) {
	OpCode a(OpCodeType::pushScope);
	a.scope = scope;
	a.size = size;
	return a;
}
OpCode OpCode::subStr(UnlinkedObj obj0, UnlinkedObj obj1, UnlinkedObj obj2) {
	OpCode a(OpCodeType::substr);
	a.obj0 = obj0;
	a.obj1 = obj1;
	a.obj2 = obj2;
	return a;
}
OpCode OpCode::clamp(UnlinkedObj obj0, UnlinkedObj obj1, UnlinkedObj obj2) {
	OpCode a(OpCodeType::clamp);
	a.obj0 = obj0;
	a.obj1 = obj1;
	a.obj2 = obj2;
	return a;
}