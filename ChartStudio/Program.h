#pragma once
#include <string>
#include <vector>
#include "Parser.h"

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


enum class UnlinkedType {
	Register,
	Variable,
	Literal,
	Null
};

enum class RegisterId {
	eax,
	ebx,
	ecx
};


enum class CodeState {
	building,
	optimized,
	ready
};

class UnlinkedObj {
	public:
		UnlinkedType unlinkedType;
		RegisterId registerId;
		IdentityType type;
		double dValue;
		string sValue;
		bool bValue;
		size_t scope;
		size_t index;

		UnlinkedObj(RegisterId registerId) {
			this->unlinkedType = UnlinkedType::Register;
			this->registerId = registerId;
		}

		UnlinkedObj(IdentityType type, size_t scope, size_t index) {
			this->unlinkedType = UnlinkedType::Variable;
			this->type = type;
			this->scope = scope;
			this->index = index;
		}

		UnlinkedObj(string value) {
			this->unlinkedType = UnlinkedType::Literal;
			this->type = IdentityType::String;
			this->sValue = value;
		}

		UnlinkedObj(double value) {
			this->unlinkedType = UnlinkedType::Literal;
			this->type = IdentityType::Double;
			this->dValue = value;
		}

		UnlinkedObj(bool value) {
			this->unlinkedType = UnlinkedType::Literal;
			this->type = IdentityType::Bool;
			this->bValue = value;
		}

		UnlinkedObj() {
			this->unlinkedType = UnlinkedType::Null;
		}
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

	OpCode(OpCodeType type) {
		this->type = type;
	}

	static OpCode ret() {
		return OpCode(OpCodeType::ret);
	}
	static OpCode id_Op(OpCodeType type, size_t id) {
		OpCode a(type);
		a.id = id;
		return a;
	}
	static OpCode label(size_t id) {
		return OpCode::id_Op(OpCodeType::label, id);
	}
	static OpCode call(size_t id) {
		return OpCode::id_Op(OpCodeType::call, id);
	}
	static OpCode exCall(size_t id) {
		return OpCode::id_Op(OpCodeType::excall, id);
	}
	static OpCode jmp(size_t id) {
		return OpCode::id_Op(OpCodeType::jmp, id);
	}
	static OpCode je(size_t id) {
		return OpCode::id_Op(OpCodeType::je, id);
	}
	static OpCode jne(size_t id) {
		return OpCode::id_Op(OpCodeType::jne, id);
	}
	static OpCode scopeDepth(size_t size) {
		OpCode a(OpCodeType::scopeDepth);
		a.size = size;
		return a;
	}

	static OpCode obj0_Op(OpCodeType type, UnlinkedObj obj0) {
		OpCode a(type);
		a.obj0 = obj0;
		return a;
	}

	static OpCode test(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::test, obj0);
	}
	static OpCode se(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::se, obj0);
	}
	static OpCode sne(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::sne, obj0);
	}
	static OpCode sa(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::sa, obj0);
	}
	static OpCode sae(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::sae, obj0);
	}
	static OpCode sb(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::sb, obj0);
	}
	static OpCode sbe(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::sbe, obj0);
	}
	static OpCode exit(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::exit, obj0);
	}
	static OpCode ceil(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::ceil, obj0);
	}
	static OpCode floor(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::floor, obj0);
	}
	static OpCode abs(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::abs, obj0);
	}
	static OpCode toDouble(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::todouble, obj0);
	}
	static OpCode toBool(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::tobool, obj0);
	}
	static OpCode len(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::len, obj0);
	}
	static OpCode lcase(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::lcase, obj0);
	}
	static OpCode ucase(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::ucase, obj0);
	}
	static OpCode trim(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::trim, obj0);
	}
	static OpCode allocDouble(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::allocDouble, obj0);
	}
	static OpCode allocBool(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::allocBool, obj0);
	}
	static OpCode allocString(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::allocString, obj0);
	}
	static OpCode push(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::push, obj0);
	}
	static OpCode pop(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::pop, obj0);
	}
	static OpCode notOp(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::notOp, obj0);
	}
	static OpCode neg(UnlinkedObj obj0) {
		return OpCode::obj0_Op(OpCodeType::neg, obj0);
	}
	static OpCode codeLine(string code) {
		OpCode a(OpCodeType::codeLine);
		a.code = code;
		return a;
	}
	static OpCode popScope(size_t scope) {
		OpCode a(OpCodeType::popScope);
		a.scope = scope;
		return a;
	}

	static OpCode obj0_and_obj1_Op(OpCodeType type, UnlinkedObj obj0, UnlinkedObj obj1) {
		OpCode a(type);
		a.obj0 = obj0;
		a.obj1 = obj1;
		return a;
	}

	static OpCode toString(UnlinkedObj obj0, UnlinkedObj obj1) {
		return OpCode::obj0_and_obj1_Op(OpCodeType::tostring, obj0, obj1);
	}
	static OpCode cmp(UnlinkedObj obj0, UnlinkedObj obj1) {
		return OpCode::obj0_and_obj1_Op(OpCodeType::cmp, obj0, obj1);
	}
	static OpCode concat(UnlinkedObj obj0, UnlinkedObj obj1) {
		return OpCode::obj0_and_obj1_Op(OpCodeType::concat, obj0, obj1);
	}
	static OpCode minOp(UnlinkedObj obj0, UnlinkedObj obj1) {
		return OpCode::obj0_and_obj1_Op(OpCodeType::min, obj0, obj1);
	}
	static OpCode maxOp(UnlinkedObj obj0, UnlinkedObj obj1) {
		return OpCode::obj0_and_obj1_Op(OpCodeType::max, obj0, obj1);
	}
	static OpCode mov(UnlinkedObj obj0, UnlinkedObj obj1) {
		return OpCode::obj0_and_obj1_Op(OpCodeType::mov, obj0, obj1);
	}
	static OpCode andOp(UnlinkedObj obj0, UnlinkedObj obj1) {
		return OpCode::obj0_and_obj1_Op(OpCodeType::andOp, obj0, obj1);
	}
	static OpCode orOp(UnlinkedObj obj0, UnlinkedObj obj1) {
		return OpCode::obj0_and_obj1_Op(OpCodeType::orOp, obj0, obj1);
	}
	static OpCode add(UnlinkedObj obj0, UnlinkedObj obj1) {
		return OpCode::obj0_and_obj1_Op(OpCodeType::add, obj0, obj1);
	}
	static OpCode sub(UnlinkedObj obj0, UnlinkedObj obj1) {
		return OpCode::obj0_and_obj1_Op(OpCodeType::sub, obj0, obj1);
	}
	static OpCode mul(UnlinkedObj obj0, UnlinkedObj obj1) {
		return OpCode::obj0_and_obj1_Op(OpCodeType::mul, obj0, obj1);
	}
	static OpCode div(UnlinkedObj obj0, UnlinkedObj obj1) {
		return OpCode::obj0_and_obj1_Op(OpCodeType::div, obj0, obj1);
	}
	static OpCode mod(UnlinkedObj obj0, UnlinkedObj obj1) {
		return OpCode::obj0_and_obj1_Op(OpCodeType::mod, obj0, obj1);
	}
	static OpCode exponent(UnlinkedObj obj0, UnlinkedObj obj1) {
		return OpCode::obj0_and_obj1_Op(OpCodeType::exponent, obj0, obj1);
	}
	static OpCode pushScope(size_t scope, size_t size) {
		OpCode a(OpCodeType::pushScope);
		a.scope = scope;
		a.size = size;
		return a;
	}
	static OpCode subStr(UnlinkedObj obj0, UnlinkedObj obj1, UnlinkedObj obj2) {
		OpCode a(OpCodeType::substr);
		a.obj0 = obj0;
		a.obj1 = obj1;
		a.obj2 = obj2;
		return a;
	}
	static OpCode clamp(UnlinkedObj obj0, UnlinkedObj obj1, UnlinkedObj obj2) {
		OpCode a(OpCodeType::clamp);
		a.obj0 = obj0;
		a.obj1 = obj1;
		a.obj2 = obj2;
		return a;
	}
};

class Program {
public:
	Program();
	~Program();

	void reset();

	size_t addCode(OpCode op);
	void insertCode(OpCode op, size_t afterThis);
	void insertCode(vector<OpCode> opCodes, size_t afterThis);

	string errorMsg;
	CodeState codeState;
	
	size_t debugCodeLine;

	vector<OpCode> code;
};



