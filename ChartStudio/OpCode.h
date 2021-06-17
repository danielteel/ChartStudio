#pragma once
#include "UnlinkedObj.h"
using namespace std;

enum class OpCodeType {
	label,
	jmp,
	cmp,
	test,
	je,
	jne,
	ja,
	jae,
	jb,
	jbe,
	se,
	sne,
	sa,
	sae,
	sb,
	sbe,
	exit,
	ceil,
	floor,
	abs,
	min,
	max,
	clamp,
	excall,
	call,
	ret,
	todouble,
	tobool,
	len,
	lcase,
	ucase,
	trim,
	substr,
	tostring,
	concat,
	allocDouble,
	allocBool,
	allocString,
	pushScope,
	popScope,
	push,
	pop,
	codeLine,
	mov,
	andOp,
	orOp,
	add,
	sub,
	mul,
	div,
	mod,
	exponent,
	notOp,
	neg,
	scopeDepth
};

class OpCode {
public:
	OpCodeType type;
	UnlinkedObj obj0;
	UnlinkedObj obj1;
	UnlinkedObj obj2;
	string code;
	size_t id;
	size_t scope;
	size_t size;

	OpCode(OpCodeType type);
	static OpCode ret();
	static OpCode id_Op(OpCodeType type, size_t id);
	static OpCode label(size_t id);
	static OpCode call(size_t id);
	static OpCode exCall(size_t id);
	static OpCode jmp(size_t id);
	static OpCode je(size_t id);
	static OpCode jne(size_t id);
	static OpCode scopeDepth(size_t size);
	static OpCode obj0_Op(OpCodeType type, UnlinkedObj obj0);
	static OpCode test(UnlinkedObj obj0);
	static OpCode se(UnlinkedObj obj0);
	static OpCode sne(UnlinkedObj obj0);
	static OpCode sa(UnlinkedObj obj0);
	static OpCode sae(UnlinkedObj obj0);
	static OpCode sb(UnlinkedObj obj0);
	static OpCode sbe(UnlinkedObj obj0);
	static OpCode exit(UnlinkedObj obj0);
	static OpCode ceil(UnlinkedObj obj0);
	static OpCode floor(UnlinkedObj obj0);
	static OpCode abs(UnlinkedObj obj0);
	static OpCode toDouble(UnlinkedObj obj0);
	static OpCode toBool(UnlinkedObj obj0);
	static OpCode len(UnlinkedObj obj0);
	static OpCode lcase(UnlinkedObj obj0);
	static OpCode ucase(UnlinkedObj obj0);
	static OpCode trim(UnlinkedObj obj0);
	static OpCode allocDouble(UnlinkedObj obj0);
	static OpCode allocBool(UnlinkedObj obj0);
	static OpCode allocString(UnlinkedObj obj0);
	static OpCode push(UnlinkedObj obj0);
	static OpCode pop(UnlinkedObj obj0);
	static OpCode notOp(UnlinkedObj obj0);
	static OpCode neg(UnlinkedObj obj0);
	static OpCode codeLine(string code);
	static OpCode popScope(size_t scope);
	static OpCode obj0_and_obj1_Op(OpCodeType type, UnlinkedObj obj0, UnlinkedObj obj1);
	static OpCode toString(UnlinkedObj obj0, UnlinkedObj obj1);
	static OpCode cmp(UnlinkedObj obj0, UnlinkedObj obj1);
	static OpCode concat(UnlinkedObj obj0, UnlinkedObj obj1);
	static OpCode minOp(UnlinkedObj obj0, UnlinkedObj obj1);
	static OpCode maxOp(UnlinkedObj obj0, UnlinkedObj obj1);
	static OpCode mov(UnlinkedObj obj0, UnlinkedObj obj1);
	static OpCode andOp(UnlinkedObj obj0, UnlinkedObj obj1);
	static OpCode orOp(UnlinkedObj obj0, UnlinkedObj obj1);
	static OpCode add(UnlinkedObj obj0, UnlinkedObj obj1);
	static OpCode sub(UnlinkedObj obj0, UnlinkedObj obj1);
	static OpCode mul(UnlinkedObj obj0, UnlinkedObj obj1);
	static OpCode div(UnlinkedObj obj0, UnlinkedObj obj1);
	static OpCode mod(UnlinkedObj obj0, UnlinkedObj obj1);
	static OpCode exponent(UnlinkedObj obj0, UnlinkedObj obj1);
	static OpCode pushScope(size_t scope, size_t size);
	static OpCode subStr(UnlinkedObj obj0, UnlinkedObj obj1, UnlinkedObj obj2);
	static OpCode clamp(UnlinkedObj obj0, UnlinkedObj obj1, UnlinkedObj obj2);
};