#include "stdafx.h"
#include "Program.h"
#include "OpObj.h"
#include "OpCode.h"

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

void Program::link() {
	if (this->codeState == CodeState::ready) return;
	
}

// 	link(optimize){
// 		if (this.codeState===Program.CodeState.READY) return null;

// 		const labelMap = new Map();
// 		for (let i=0;i<this.code.length;i++){//Make a map of all the labels and there indexes
// 			if (this.code[i].type===OpCode.label){
// 				if (labelMap.has(this.code[i].id)){
// 					this.otherError("error linking, "+this.code[i].id+" was already defined")
// 				}
// 				labelMap.set(this.code[i].id, i);

// 				this.code.splice(i,1);
// 				i--;
// 			}
// 		}

// 		for (let i=0;i<this.code.length;i++){//Now go through and update the id's of all the codes that can/will jump
// 			switch (this.code[i].type){
// 				case OpCode.jmp:
// 				case OpCode.je:
// 				case OpCode.jne:
// 				case OpCode.ja:
// 				case OpCode.jae:
// 				case OpCode.jb:
// 				case OpCode.jbe:
// 				case OpCode.call:
// 					this.code[i].id=labelMap.get(this.code[i].id);
// 					break;
// 			}
// 		}
// 		this.codeState=Program.CodeState.READY;
// 	}

// 	linkedObject(obj, scopes){
// 		switch (obj.type){
// 			case UnlinkedType.register:
// 				switch (obj.register){
// 					case Program.regSymbols.eax:
// 						return this.eax;
// 					case Program.regSymbols.ebx:
// 						return this.ebx;
// 					case Program.regSymbols.ecx:
// 						return this.ecx;
// 				}
// 				this.executionError("unknown register");

// 			case UnlinkedType.variable:
// 				return scopes[obj.scope][scopes[obj.scope].length-1][obj.index];

// 			case UnlinkedType.literal:
// 				switch (obj.literalType){
// 					case IdentityType.Bool:
// 						if (obj.value===true) return this.true;
// 						return this.false;
// 					case IdentityType.Double:
// 						return new NumberObj(null, obj.value, true);
// 					case IdentityType.String:
// 						return new StringObj(null, obj.value, true);
// 					default:
// 						this.executionError("unknown unlinked literal type");
// 				}

// 			case UnlinkedType.null:
// 				return this.null;
// 		}		
// 		this.executionError("unknown unlinked object type");
// 	}

// 	otherError(message){
// 		throw Error("Pre-execution error: "+message);
// 	}

// 	executionError(message){
// 		throw Error(message);
// 	}

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