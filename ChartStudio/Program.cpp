#include <map>
#include "stdafx.h"
#include "Program.h"
#include "OpObj.h"
#include "OpCode.h"

using namespace std;

Program::Program() {
	this->errorMsg = "";

	this->code.clear();
	this->codeState = CodeState::building;

	this->debugCodeLine = 1;

}


Program::~Program() {
}

void Program::reset() {
	this->errorMsg = "";
	this->code.clear();
	this->codeState = CodeState::building;
	this->debugCodeLine = 1;
}

size_t Program::addCode(OpCode op) {
	this->code.push_back(op);
	return this->code.size();
}

void Program::insertCode(OpCode op, size_t afterThis) {
	vector<OpCode>::iterator whereTo = this->code.begin() + afterThis;
	this->code.insert(whereTo, op);
}

void Program::insertCode(vector<OpCode> opCodes, size_t afterThis) {
	if (opCodes.size() == 0) return;
	for (size_t i = opCodes.size(); i > 0; i--) {
		this->insertCode(opCodes[i-1], afterThis);
	}
}
/*
void Program::link() {
	if (this->codeState == CodeState::ready) return;
	
	map<size_t, size_t> labelMap;

	for (size_t i = 0; i < this->code.size(); i++) {
		if (this->code[i].type == OpCodeType::label) {
			if (labelMap.find(this->code[i].id)!=labelMap.end()) {
				throw "link error duplicate label define";
			}

			labelMap.insert(std::pair<size_t, size_t>(this->code[i].id, i));
		}
	}

	for (size_t i=0;i<this->code.size();i++){//Now go through and update the id's of all the codes that can/will jump
 			switch (this->code[i].type){
				case OpCodeType::jmp:
 				case OpCodeType::je:
 				case OpCodeType::jne:
 				case OpCodeType::ja:
 				case OpCodeType::jae:
 				case OpCodeType::jb:
 				case OpCodeType::jbe:
 				case OpCodeType::call:
 					this->code[i].id=labelMap[this->code[i].id];
 					break;
 			}
 	}
	this->codeState = CodeState::ready;
}

OpObj* Program::linkedObject(UnlinkedObj* obj, vector<vector<vector<OpObj*>>>& scopes, bool* needToFree) {
	if (needToFree) needToFree = false;

	switch (obj->unlinkedType) {
		case UnlinkedType::Register:
			switch (obj->registerId) {
				case RegisterId::eax:
					return &this->eax;
				case RegisterId::ebx:
					return &this->ebx;
				case RegisterId::ecx:
					return &this->ecx;
			}
			throw "unknown register";

		case UnlinkedType::Variable:
			return &(scopes[obj->scope][scopes[obj->scope].size() - 1][obj->index]);

		case UnlinkedType::Literal:
			switch (obj->type) {
				case IdentityType::Bool:
					if (obj->bValue) return &this->trueObj;
					return &this->falseObj;
				case IdentityType::String:
					if (needToFree) *needToFree = true;
					return new StringObj(obj->sValue, true);
				case IdentityType::Double:
					if (needToFree) *needToFree = true;
					return new NumberObj(obj->dValue, true);
				case IdentityType::Null:
					return &this->nullObj;
				default:
					throw "unknown unlinked object literal type";
			}

		case UnlinkedType::Null:
			return &this->nullObj;

		default:
			throw "unknown unlinked object type";
	}
}


OpObj* Program::execute(vector<OpObj*> externals) {
	size_t eip = 0;
	try {
		if (this->codeState != CodeState::ready) {
			this->link();
		}
		if (this->codeState != CodeState::ready) throw "failed to link code";

		this->debugCodeLine = 1;
		bool notDone = true;

		vector<vector<vector<OpObj*>>> scopes;
		scopes.push_back({});
		scopes[0].push_back({});
		(scopes[0])[0] = externals;

		vector<size_t> callStack;
		vector<OpObj*> stack;

		bool flag_e = false;
		bool flag_a = false;
		bool flag_b = false;

		while (notDone && eip < this->code.size()) {
			OpCode op = this->code[eip];
			OpObj* objs[3] = { nullptr, nullptr, nullptr };
			bool freeObjs[3] = { false, false, false };

		}
	}


}*/
// 	execute(externals){
// 		let eip = 0;
// 		let trace=[];
// 		try {
// 			const link = (obj) => this.linkedObject(obj, scopes);

// 			if (this.codeState!==Program.CodeState.READY) this.executionError("tried executing on unlinked code")

// 			this.debugCodeLine=1;

// 			let notDone=true;

// 			let scopes=[[externals]];

// 			let callStack=[];
// 			let stack=[];

// 			let flag_e=false;
// 			let flag_a=false;
// 			let flag_b=false;

// 			while (notDone && eip<this.code.length){
// 				let opcode=this.code[eip];
// 				let obj0=null;
// 				let obj1=null;
// 				let obj2=null;
// 				switch (opcode.type){
// 					case OpCode.label:
// 						break;

// 					case OpCode.excall:
// 						this.eax.setTo(externals[opcode.id]( () => stack.pop() ));
// 						break;
// 					case OpCode.call:
// 						trace.push({debugName: opcode.debugName, address: opcode.id});
// 						callStack.push(eip+1);
// 						eip=opcode.id;
// 						continue;
// 					case OpCode.ret:
// 						trace.pop();
// 						eip=callStack.pop();
// 						continue;

// 					case OpCode.exit:
// 						return (link(opcode.obj0).getCopy(true));

// 					case OpCode.jmp:
// 						eip=opcode.id;
// 						continue;
// 					case OpCode.test:
// 						flag_e=!link(opcode.obj0).value;
// 						break;
// 					case OpCode.cmp:
// 						obj0=link(opcode.obj0);
// 						obj1=link(opcode.obj1);
// 						flag_e=obj0.eqaulTo(obj1);
// 						if (((obj0.objType===OpObjType.register && obj0._curValType===OpObjType.num) || obj0.objType===OpObjType.num) &&
// 							((obj1.objType===OpObjType.register && obj1._curValType===OpObjType.num) || obj1.objType===OpObjType.num)){

// 							flag_a=obj0.greaterThan(obj1);
// 							flag_b=obj0.smallerThan(obj1);
// 						}else{
// 							flag_a=false;
// 							flag_b=false;
// 						}
// 						break;

// 					case OpCode.je:
// 						if (flag_e){
// 							eip=opcode.id;
// 							continue;
// 						}
// 						break;
// 					case OpCode.jne:
// 						if (!flag_e){
// 							eip=opcode.id;
// 							continue;
// 						}
// 						break;
// 					case OpCode.ja:
// 						if (flag_a){
// 							eip=opcode.id;
// 							continue;
// 						}
// 						break;
// 					case OpCode.jae:
// 						if (flag_a || flag_e){
// 							eip=opcode.id;
// 							continue;
// 						}
// 						break;
// 					case OpCode.jb:
// 						if (flag_b){
// 							eip=opcode.id;
// 							continue;
// 						}
// 						break;
// 					case OpCode.jbe:
// 						if (flag_b || flag_e){
// 							eip=opcode.id;
// 							continue;
// 						}
// 						break;

// 					case OpCode.se:
// 						link(opcode.obj0).setTo(flag_e?this.true:this.false)
// 						break;
// 					case OpCode.sne:
// 						link(opcode.obj0).setTo(!flag_e?this.true:this.false)
// 						break;
// 					case OpCode.sa:
// 						link(opcode.obj0).setTo(flag_a?this.true:this.false)
// 						break;
// 					case OpCode.sae:
// 						link(opcode.obj0).setTo(flag_a||flag_e?this.true:this.false)
// 						break;
// 					case OpCode.sb:
// 						link(opcode.obj0).setTo(flag_b?this.true:this.false)
// 						break;
// 					case OpCode.sbe:
// 						link(opcode.obj0).setTo(flag_b||flag_e?this.true:this.false)
// 						break;

// 					case OpCode.ceil:
// 						obj0 = link(opcode.obj0);
// 						if (obj0.value===null) this.executionError("tried to do ceil on null value");
// 						obj0.setTo(new NumberObj(null, Math.ceil(obj0.value), true));
// 						break;
// 					case OpCode.floor:
// 						obj0 = link(opcode.obj0);
// 						if (obj0.value===null) this.executionError("tried to do floor on null value");
// 						obj0.setTo(new NumberObj(null, Math.floor(obj0.value), true));
// 						break;
// 					case OpCode.abs:
// 						obj0 = link(opcode.obj0);
// 						if (obj0.value===null) this.executionError("tried to do abs on null value");
// 						obj0.setTo(new NumberObj(null, Math.abs(obj0.value), true));
// 						break;
// 					case OpCode.min:
// 						obj0 = link(opcode.obj0);
// 						obj1 = link(opcode.obj1);
// 						if (obj0.value===null) this.executionError("tried to do min on null value");	
// 						if (obj1.value===null) this.executionError("tried to do min on null value");	
// 						obj0.setTo( new NumberObj(null, Math.min(obj0.value, obj1.value), true) );
// 						break;
// 					case OpCode.max:
// 						obj0 = link(opcode.obj0);
// 						obj1 = link(opcode.obj1);
// 						if (obj0.value===null) this.executionError("tried to do max on null value");	
// 						if (obj1.value===null) this.executionError("tried to do max on null value");	
// 						obj0.setTo( new NumberObj(null, Math.max(obj0.value, obj1.value), true) );
// 						break;
// 					case OpCode.clamp:
// 						obj0 = link(opcode.obj0);
// 						obj1 = link(opcode.obj1);
// 						obj2 = link(opcode.obj2);
// 						if (obj0.value===null) this.executionError("tried to do clamp on null value");
// 						if (obj1.value===null) this.executionError("tried to do clamp on null value");
// 						if (obj2.value===null) this.executionError("tried to do clamp on null value");
// 						obj0.setTo( new NumberObj(null, Math.min(Math.max(obj0.value, obj1.value), obj2.value), true) );
// 						break;
// 					case OpCode.todouble:
// 						obj0 = link(opcode.obj0);
// 						if (obj0.value===null) this.executionError("tried to convert null to double");	
// 						obj0.setTo(new NumberObj(null, Number(obj0.value), true));
// 						break;
// 					case OpCode.tobool:
// 						obj0 = link(opcode.obj0);
// 						if (obj0.value===null) this.executionError("tried to convert null to bool");
// 						obj0.setTo(new BoolObj(null, Boolean(obj0.value), true));
// 						break;
// 					case OpCode.len:
// 						obj0 = link(opcode.obj0);
// 						if (obj0.value===null) this.executionError("tried to get length of null string");
// 						obj0.setTo(new NumberObj(null, obj0.value.length, true));
// 						break;

// 					case OpCode.lcase:
// 						obj0 = link(opcode.obj0);
// 						if (obj0.value===null) this.executionError("tried to set null string to lower case");
// 						obj0.setTo( new StringObj(null, obj0.value.toLowerCase(), true) );
// 						break;
// 					case OpCode.ucase:
// 						obj0 = link(opcode.obj0);
// 						if (obj0.value===null) this.executionError("tried to set null string to upper case");
// 						obj0.setTo( new StringObj(null, obj0.value.toUpperCase(), true) );
// 						break;
// 					case OpCode.trim:
// 						obj0 = link(opcode.obj0);
// 						if (obj0.value===null) this.executionError("tried to trim null string");
// 						obj0.setTo( new StringObj(null, obj0.value.trim(), true) );
// 						break;
// 					case OpCode.substr:
// 						obj0 = link(opcode.obj0);
// 						obj1 = link(opcode.obj1);
// 						obj2 = link(opcode.obj2);
// 						if (obj0.value===null) this.executionError("tried to get substring of null string");
// 						if (obj1.value===null) this.executionError("tried to get substring with null index");
// 						if (obj2.value===null) this.executionError("tried to get substring with null length");
// 						obj0.setTo( new StringObj(null, obj0.value.substr(obj1.value, obj2.value), true) );
// 						break;
// 					case OpCode.tostring:
// 						obj0 = link(opcode.obj0);
// 						obj1 = link(opcode.obj1);
// 						if (obj0.value===null) this.executionError("tried to convert null to string");
// 						let val=obj0.value;
// 						if (obj1.value!==null){
// 							if (obj1.value>=0) val=Number(val).toFixed(obj1.value);
// 							if (obj1.value<0){
// 								const multiplier=10**Math.abs(obj1.value);
// 								val=String(Math.round(val/multiplier)*multiplier);
// 							}
// 						}else{
// 							val=String(val);
// 						}
// 						obj0.setTo( new StringObj(null, val, true) );
// 						break;
// 					case OpCode.concat:
// 						obj0 = link(opcode.obj0);
// 						obj1 = link(opcode.obj1);
// 						if (obj0.value===null) this.executionError("tried to concat null to string");
// 						if (obj1.value===null) this.executionError("tried to concat string to null");
// 						obj0.setTo( new StringObj(null, obj0.value+obj1.value, true) );
// 						break;
// 					case OpCode.double:
// 						scopes[opcode.obj0.scope][scopes[opcode.obj0.scope].length-1][opcode.obj0.index]=new NumberObj(null, null, false);
// 						break;
// 					case OpCode.bool:
// 						scopes[opcode.obj0.scope][scopes[opcode.obj0.scope].length-1][opcode.obj0.index]=new BoolObj(null, null, false);
// 						break;
// 					case OpCode.string:
// 						scopes[opcode.obj0.scope][scopes[opcode.obj0.scope].length-1][opcode.obj0.index]=new StringObj(null, null, false);
// 						break;
// 					case OpCode.pushscope:
// 						scopes[opcode.scope].push(new Array(opcode.size));
// 						break;
// 					case OpCode.popscope:
// 						scopes[opcode.scope].pop();
// 						break;
// 					case OpCode.push:
// 						stack.push(link(opcode.obj0).getCopy());
// 						break;
// 					case OpCode.pop:
// 						link(opcode.obj0).setTo(stack.pop());
// 						break;
// 					case OpCode.codeline:
// 						//essentially a nop
// 						//used for debugging, shows when there was a line break in the original code
// 						if (opcode.code!==null){
// 							this.debugCodeLine++;
// 						}
// 						break;
// 					case OpCode.mov:
// 						link(opcode.obj0).setTo(link(opcode.obj1));
// 						break;
// 					case OpCode.and:
// 						obj0=link(opcode.obj0);
// 						obj0.setTo( new BoolObj(null, obj0.value && link(opcode.obj1).value, true) );
// 						break;
// 					case OpCode.or:
// 						obj0=link(opcode.obj0);
// 						obj0.setTo( new BoolObj(null, obj0.value || link(opcode.obj1).value, true) );
// 						break;
// 					case OpCode.add:
// 						obj0=link(opcode.obj0);
// 						obj1=link(opcode.obj1);
// 						if (obj0.value===null || obj1.value===null) this.executionError("tried to add null");
// 						obj0.setTo( new NumberObj(null, obj0.value + obj1.value, true) );
// 						break;
// 					case OpCode.sub:
// 						obj0=link(opcode.obj0);
// 						obj1=link(opcode.obj1);
// 						if (obj0.value===null || obj1.value===null) this.executionError("tried to sub null");
// 						obj0.setTo( new NumberObj(null, obj0.value - obj1.value, true) );
// 						break;
// 					case OpCode.mul:
// 						obj0=link(opcode.obj0);
// 						obj1=link(opcode.obj1);
// 						obj0.setTo( new NumberObj(null, obj0.value * obj1.value, true) );
// 						break;
// 					case OpCode.div:
// 						obj0=link(opcode.obj0);
// 						obj1=link(opcode.obj1);
// 						if (obj0.value===null || obj1.value===null) this.executionError("tried to div with a null value");

// 						if (Utils.isAboutEquals(obj1.value,0)){//divide by zero! set it to null
// 							obj0.setTo( this.null );
// 						}else{
// 							obj0.setTo( new NumberObj(null, obj0.value / obj1.value, true) );
// 						}
// 						break;
// 					case OpCode.mod:
// 						obj0=link(opcode.obj0);
// 						obj1=link(opcode.obj1);
// 						if (obj0.value===null || obj1.value===null) this.executionError("tried to mod with a null value");

// 						if (Utils.isAboutEquals(obj1.value,0)){//divide by zero! set it to null
// 							obj0.setTo( this.null );
// 						}else{
// 							obj0.setTo( new NumberObj(null, obj0.value % obj1.value, true) );
// 						}
// 						break;
// 					case OpCode.exponent:
// 						obj0=link(opcode.obj0);
// 						obj1=link(opcode.obj1);
// 						if (obj0.value===null || obj1.value===null) this.executionError("tried to do exponentiation with a null value");
// 						obj0.setTo( new NumberObj(null, obj0.value ** obj1.value, true) );
// 						break;
// 					case OpCode.not:
// 						obj0=link(opcode.obj0);
// 						if (obj0.value===null) this.executionError("tried to not a null value");
// 						obj0.setTo( new BoolObj(null, !obj0.value, true) );
// 						break;
// 					case OpCode.neg:
// 						obj0=link(opcode.obj0);
// 						if (obj0.value===null) this.executionError("tried to neg null value");
// 						obj0.setTo( new NumberObj(null, 0-obj0.value, true) );
// 						break;
// 					case OpCode.scopedepth:
// 						for (let i=0;i<opcode.size;i++){
// 							scopes.push([[]]);
// 						}
// 						break;
// 				}
// 				eip++;
// 			}

// 			return new BoolObj(null, true, true);
// 		} catch (error) {
// 			let callTraceMsg="\n";
// 			for (let callData of trace.reverse()){
// 				callTraceMsg+="\tat "+callData.debugName+" address: "+callData.address+"\n";
// 			}
// 			throw Error("Execution error at address "+eip+": "+error.message+"\n"+callTraceMsg);
// 		}
// 	}