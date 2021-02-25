#include "stdafx.h"
#include "Program.h"
#include "OpObj.h"


Program::Program() {
	this->errorMsg = "";

	this->code.clear();
	this->codeState = CodeState::building;

	this->debugCodeLine = 1;

}


Program::~Program() {
}

OpCode Program::addCode(OpCode op) {
	this->code.push_back(op);
}

OpCode Program::insertCode(OpCode op, size_t afterThis) {
	vector<OpCode>::iterator whereTo = this->code.begin() + afterThis;
	this->code.insert(whereTo, op);
}

OpCode Program::insertCode(vector<OpCode> opCodes, size_t afterThis) {
	vector<OpCode>::iterator whereTo = this->code.begin() + afterThis;
	for (size_t i = opCodes.size() - 1; i >= 0; i--) {
		this->code.insert(whereTo, opCodes[i]);
	}
}


// 	addRet			(dontInsert=false)						{ const op={type: OpCode.ret};return dontInsert?op:this.code.push( op )-1; }
// 	addLabel		(id,dontInsert=false)					{ const op={type: OpCode.label,		id: id};return dontInsert?op:this.code.push( op )-1; }
// 	addJmp			(id,dontInsert=false)					{ const op={type: OpCode.jmp,		id: id};return dontInsert?op:this.code.push( op )-1; }
// 	addJE			(id,dontInsert=false)					{ const op={type: OpCode.je,		id: id};return dontInsert?op:this.code.push( op )-1; }
// 	addJNE			(id,dontInsert=false)					{ const op={type: OpCode.jne,		id: id};return dontInsert?op:this.code.push( op )-1; }
// 	addScopeDepth	(size,dontInsert=false)					{ const op={type: OpCode.scopedepth,size: size};return dontInsert?op:this.code.push( op )-1; }
// 	addTest			(obj0,dontInsert=false)					{ const op={type: OpCode.test,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addSE			(obj0,dontInsert=false)					{ const op={type: OpCode.se,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addSNE			(obj0,dontInsert=false)					{ const op={type: OpCode.sne,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addSA			(obj0,dontInsert=false)					{ const op={type: OpCode.sa,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addSAE			(obj0,dontInsert=false)					{ const op={type: OpCode.sae,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addSB			(obj0,dontInsert=false)					{ const op={type: OpCode.sb,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addSBE			(obj0,dontInsert=false)					{ const op={type: OpCode.sbe,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addExit			(obj0,dontInsert=false)					{ const op={type: OpCode.exit,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addCeil			(obj0,dontInsert=false)					{ const op={type: OpCode.ceil,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addFloor		(obj0,dontInsert=false)					{ const op={type: OpCode.floor,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addAbs			(obj0,dontInsert=false)					{ const op={type: OpCode.abs,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addToDouble		(obj0,dontInsert=false)					{ const op={type: OpCode.todouble,	obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addToBool		(obj0,dontInsert=false)					{ const op={type: OpCode.tobool,	obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addLen			(obj0,dontInsert=false)					{ const op={type: OpCode.len,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addLCase		(obj0,dontInsert=false)					{ const op={type: OpCode.lcase,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addUCase		(obj0,dontInsert=false)					{ const op={type: OpCode.ucase,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addTrim			(obj0,dontInsert=false)					{ const op={type: OpCode.trim,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addDouble		(obj0,dontInsert=false)					{ const op={type: OpCode.double,	obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addBool			(obj0,dontInsert=false)					{ const op={type: OpCode.bool,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addString		(obj0,dontInsert=false)					{ const op={type: OpCode.string,	obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addPush			(obj0,dontInsert=false)					{ const op={type: OpCode.push,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addPop			(obj0,dontInsert=false)					{ const op={type: OpCode.pop,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addNot			(obj0,dontInsert=false)					{ const op={type: OpCode.not,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addNeg			(obj0,dontInsert=false)					{ const op={type: OpCode.neg,		obj0: obj0};return dontInsert?op:this.code.push( op )-1; }
// 	addCodeLine		(code,dontInsert=false)					{ const op={type: OpCode.codeline,	code: code};return dontInsert?op:this.code.push( op )-1; }
// 	addPopScope		(scope,dontInsert=false)				{ const op={type: OpCode.popscope,	scope: scope};return dontInsert?op:this.code.push( op )-1; }
// 	addToString		(obj0, obj1,dontInsert=false)			{ const op={type: OpCode.tostring,	obj0: obj0, obj1: obj1};return dontInsert?op:this.code.push( op )-1; }
// 	addCmp			(obj0, obj1,dontInsert=false)			{ const op={type: OpCode.cmp,		obj0: obj0, obj1: obj1};return dontInsert?op:this.code.push( op )-1; }
// 	addConcat		(obj0, obj1,dontInsert=false)			{ const op={type: OpCode.concat,	obj0: obj0, obj1: obj1};return dontInsert?op:this.code.push( op )-1; }
// 	addMin			(obj0, obj1,dontInsert=false)			{ const op={type: OpCode.min,		obj0: obj0, obj1: obj1};return dontInsert?op:this.code.push( op )-1; }
// 	addMax			(obj0, obj1,dontInsert=false)			{ const op={type: OpCode.max,		obj0: obj0, obj1: obj1};return dontInsert?op:this.code.push( op )-1; }
// 	addMov			(obj0, obj1,dontInsert=false)			{ const op={type: OpCode.mov,		obj0: obj0, obj1: obj1};return dontInsert?op:this.code.push( op )-1; }
// 	addAnd			(obj0, obj1,dontInsert=false)			{ const op={type: OpCode.and,		obj0: obj0, obj1: obj1};return dontInsert?op:this.code.push( op )-1; }
// 	addOr			(obj0, obj1,dontInsert=false)			{ const op={type: OpCode.or,		obj0: obj0, obj1: obj1};return dontInsert?op:this.code.push( op )-1; }
// 	addAdd			(obj0, obj1,dontInsert=false)			{ const op={type: OpCode.add,		obj0: obj0, obj1: obj1};return dontInsert?op:this.code.push( op )-1; }
// 	addSub			(obj0, obj1,dontInsert=false)			{ const op={type: OpCode.sub,		obj0: obj0, obj1: obj1};return dontInsert?op:this.code.push( op )-1; }
// 	addMul			(obj0, obj1,dontInsert=false)			{ const op={type: OpCode.mul,		obj0: obj0, obj1: obj1};return dontInsert?op:this.code.push( op )-1; }
// 	addDiv			(obj0, obj1,dontInsert=false)			{ const op={type: OpCode.div,		obj0: obj0, obj1: obj1};return dontInsert?op:this.code.push( op )-1; }
// 	addMod			(obj0, obj1,dontInsert=false)			{ const op={type: OpCode.mod,		obj0: obj0, obj1: obj1};return dontInsert?op:this.code.push( op )-1; }
// 	addExponent		(obj0, obj1,dontInsert=false)			{ const op={type: OpCode.exponent,	obj0: obj0, obj1: obj1};return dontInsert?op:this.code.push( op )-1; }
// 	addPushScope	(scope, size,dontInsert=false)			{ const op={type: OpCode.pushscope,	scope: scope, size: size};return dontInsert?op:this.code.push( op )-1; }
// 	addCall			(id, debugName,dontInsert=false)		{ const op={type: OpCode.call,		id: id, debugName: debugName};return dontInsert?op:this.code.push( op )-1; }
// 	addExCall		(id, debugName,dontInsert=false)		{ const op={type: OpCode.excall,	id: id, debugName: debugName};return dontInsert?op:this.code.push( op )-1; }
// 	addSubStr		(obj0, obj1, obj2,dontInsert=false)		{ const op={type: OpCode.substr,	obj0: obj0, obj1: obj1, obj2: obj2};return dontInsert?op:this.code.push( op )-1; }
// 	addClamp		(obj0, obj1, obj2,dontInsert=false)		{ const op={type: OpCode.clamp,		obj0: obj0, obj1: obj1, obj2: obj2};return dontInsert?op:this.code.push( op )-1; }


