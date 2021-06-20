#include "stdafx.h"
#include <map>
#include "Program.h"
#include "OpObj.h"
#include "OpCode.h"
#include "helpers.h"
#include <iostream>
#include <math.h> 
#include <algorithm>
#include <cctype>

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

void Program::throwError(string message) {
	this->errorMsg = "Execution error: " + message;
	throw 'E';
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
	
	map<size_t, size_t> labelMap;


	for (size_t i = 0; i < this->code.size(); i++) {
		if (this->code[i].type == OpCodeType::label) {
			if (labelMap.find(this->code[i].id)!=labelMap.end()) {
				this->throwError("link error duplicate label define");
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
					if (labelMap.find(this->code[i].id) != labelMap.end()) {
						this->code[i].id = labelMap[this->code[i].id];
					} else {
						this->throwError("link error, unknown label id");
					}
 					break;
 			}
 	}
	this->codeState = CodeState::ready;
}

OpObj* Program::linkedObject(UnlinkedObj* obj, vector<vector<vector<OpObj*>>>& scopes, bool* needToFree) {
	if (needToFree) *needToFree = false;

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
			this->throwError("unknown register");

		case UnlinkedType::Variable:
			return (scopes[obj->scope][scopes[obj->scope].size() - 1][obj->index]);

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
					this->throwError("unknown unlinked object literal type");
			}

		case UnlinkedType::Null:
			return &this->nullObj;

		default:
			this->throwError("unknown unlinked object type");
	}
}


//Hacky way to get this to work, dont recommend
vector<OpObj*>* stackForPoppin= nullptr;
OpObj* popStack() {
	if (!stackForPoppin) return nullptr;
	OpObj* obj = stackForPoppin->back();
	stackForPoppin->pop_back();
	return obj;
}

void cleanUp(vector<vector<vector<OpObj*>>>& scopes, vector<OpObj*>& cleanUpStack, vector<OpObj*>& stack, OpObj** objs, bool* freeObjs) {
	stackForPoppin = nullptr;
	for (std::vector<vector<vector<OpObj*>>>::iterator it = ++scopes.begin(); it != scopes.end(); ++it) {
		for (auto scope2 : *it) {
			for (auto obj : scope2) {
				if (obj) delete obj;
			}
		}
	}
	for (auto obj : cleanUpStack) {
		delete obj;
	}
	for (auto obj : stack) {
		delete obj;
	}
	if (freeObjs[0] && objs[0]) { delete objs[0]; objs[0] = nullptr; freeObjs[0] = false; }
	if (freeObjs[1] && objs[1]) { delete objs[1]; objs[1] = nullptr; freeObjs[1] = false; }
	if (freeObjs[2] && objs[2]) { delete objs[2]; objs[2] = nullptr; freeObjs[2] = false; }
}

OpObj* Program::execute(vector<ExternalDef> externals) {
	size_t eip = 0;
	vector<OpObj*> cleanUpStack;


	OpObj* objs[3] = { nullptr, nullptr, nullptr };
	bool freeObjs[3] = { false, false, false };
	vector<OpObj*> stack;

	vector<OpObj*> execExternList;

	for (ExternalDef obj : externals) {
		if (obj.type == IdentityType::Function) {
			FuncObj* newFunc = new FuncObj(obj.functionAddress);
			cleanUpStack.push_back(newFunc);
			execExternList.push_back(newFunc);
		} else {
			execExternList.push_back(obj.opObj);
		}
	}

	vector<vector<vector<OpObj*>>> scopes = { {execExternList} };

	try {

		if (this->codeState != CodeState::ready) {
			this->link();
		}
		if (this->codeState != CodeState::ready) this->throwError("failed to link code");

		this->debugCodeLine = 1;
		bool notDone = true;


		vector<size_t> callStack;
		stackForPoppin = &stack;



		bool flag_e = false;
		bool flag_a = false;
		bool flag_b = false;

		OpCode op(OpCodeType::label);


		while (notDone && eip < this->code.size()) {
			op = this->code[eip];
			objs[0] = nullptr;
			objs[1] = nullptr;
			objs[2] = nullptr;
			freeObjs[0] = false;
			freeObjs[1] = false;
			freeObjs[2] = false;

			switch (op.type) {
				case OpCodeType::label:
					break;
				case OpCodeType::codeLine:
					break;
				case OpCodeType::excall:
					{
						OpObj* funcObj = execExternList[op.id];
						if (funcObj->valueType != OpObjType::Function) {
							this->throwError("tried to excall non function object");
						}
						objs[0] = static_cast<FuncObj*>(funcObj)->fn(&popStack);
						freeObjs[0] = true;
						this->eax.setTo(objs[0]);
					}
					break;
				case OpCodeType::call:
					callStack.push_back(eip+1);
					eip=op.id;
					continue;
				case OpCodeType::ret:
					eip=callStack.back();
					callStack.pop_back();
					continue;
				case OpCodeType::jmp:
					eip = op.id;
					continue;
				case OpCodeType::je:
					if (flag_e) {
						eip=op.id;
						continue;
					}
					break;
				case OpCodeType::jne:
					if (!flag_e) {
						eip = op.id;
						continue;
					}
					break;
				case OpCodeType::ja:
					if (flag_a) {
						eip = op.id;
						continue;
					}
					break;
				case OpCodeType::jae:
					if (flag_a || flag_e) {
						eip = op.id;
						continue;
					}
					break;
				case OpCodeType::jb:
					if (flag_b) {
						eip = op.id;
						continue;
					}
					break;
				case OpCodeType::jbe:
					if (flag_b || flag_e) {
						eip = op.id;
						continue;
					}
					break;
				case OpCodeType::cmp:
					objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
					objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
					flag_e = objs[0]->equalTo(objs[1]);
					if (((objs[0]->objType == OpObjType::Register && objs[0]->valueType == OpObjType::Number) || objs[0]->objType == OpObjType::Number) &&
						((objs[1]->objType == OpObjType::Register && objs[1]->valueType == OpObjType::Number) || objs[1]->objType == OpObjType::Number)) {

						flag_a = objs[0]->greaterThan(objs[1]);
						flag_b = objs[0]->smallerThan(objs[1]);
					} else {
						flag_a = false;
						flag_b = false;
					}
					break;

				case OpCodeType::test:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						OpObj* objToTest = objs[0];
						if (objToTest->objType == OpObjType::Register) {
							objToTest = static_cast<RegisterObj*>(objs[0])->getNativeObj();
						}
						switch (objToTest->objType) {
						case OpObjType::Bool:
							flag_e = !*static_cast<BoolObj*>(objToTest)->value;
							break;
						case OpObjType::Null:
							flag_e = true;
							break;
						case OpObjType::Number:
							flag_e = !*static_cast<NumberObj*>(objToTest)->value;
							break;
						case OpObjType::String:
							flag_e = !static_cast<StringObj*>(objToTest)->value->size();
							break;
						default:
							this->throwError("unknown test type");
						}
					}
					break;
				case OpCodeType::se:
					objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
					objs[0]->setTo(flag_e ? &this->trueObj : &this->falseObj);
					break;
				case OpCodeType::sne:
					objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
					objs[0]->setTo(flag_e ? &this->falseObj : &this->trueObj);
					break;
				case OpCodeType::sa:
					objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
					objs[0]->setTo(flag_a ? &this->trueObj : &this->falseObj);
					break;
				case OpCodeType::sae:
					objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
					objs[0]->setTo((flag_a||flag_e) ? &this->trueObj : &this->falseObj);
					break;
				case OpCodeType::sb:
					objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
					objs[0]->setTo(flag_b ? &this->trueObj : &this->falseObj);
					break;
				case OpCodeType::sbe:
					objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
					objs[0]->setTo((flag_b || flag_e) ? &this->trueObj : &this->falseObj);
					break;
				case OpCodeType::exit:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						OpObj* returnObj = objs[0]->getCopy();

						cleanUp(scopes, cleanUpStack, stack, objs, freeObjs);
						return returnObj;
					}
					break;

				case OpCodeType::allocDouble:
					if (scopes[op.obj0.scope][scopes[op.obj0.scope].size() - 1][op.obj0.index]) delete scopes[op.obj0.scope][scopes[op.obj0.scope].size() - 1][op.obj0.index];
					scopes[op.obj0.scope][scopes[op.obj0.scope].size() - 1][op.obj0.index] = new NumberObj(nullopt, false);
					break;
				case OpCodeType::allocBool:
					if (scopes[op.obj0.scope][scopes[op.obj0.scope].size() - 1][op.obj0.index]) delete scopes[op.obj0.scope][scopes[op.obj0.scope].size() - 1][op.obj0.index];
					scopes[op.obj0.scope][scopes[op.obj0.scope].size() - 1][op.obj0.index] = new BoolObj(nullopt, false);
					break;
				case OpCodeType::allocString:
					if (scopes[op.obj0.scope][scopes[op.obj0.scope].size() - 1][op.obj0.index]) delete scopes[op.obj0.scope][scopes[op.obj0.scope].size() - 1][op.obj0.index];
					scopes[op.obj0.scope][scopes[op.obj0.scope].size() - 1][op.obj0.index] = new StringObj(nullopt, false);
					break;
				case OpCodeType::pushScope:
					scopes[op.scope].push_back({});
					scopes[op.scope][scopes[op.scope].size()-1].resize(op.size);
					break;
				case OpCodeType::popScope:
					for (auto obj : scopes[op.scope][scopes[op.scope].size()-1]) {
							if (obj) delete obj;
					}
					scopes[op.scope].pop_back();
					break;
				case OpCodeType::push:
					objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
					stack.push_back(objs[0]->getCopy());
					break;
				case OpCodeType::pop:
					objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
					objs[0]->setTo(stack.back());
					delete stack.back();
					stack.pop_back();
					break;
				case OpCodeType::ceil:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						if (objs[0]->valueType != OpObjType::Number) {
							this->throwError("tried operating on non number value ceil");
						}
						NumberObj* num = static_cast<NumberObj*>(objs[0]->getCopy());
						if (num->value == nullopt) {
							delete num;
							this->throwError("tried operating on null value");
						}
						num->value = ceil(*num->value);
						objs[0]->setTo(num);
						delete num;
					}
					break;
				case OpCodeType::floor:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						if (objs[0]->valueType != OpObjType::Number) {
							this->throwError("tried operating on non number value floor");
						}
						NumberObj* num = static_cast<NumberObj*>(objs[0]->getCopy());
						if (num->value == nullopt) {
							delete num;
							this->throwError("tried operating on null value");
						}
						num->value = floor(*num->value);
						objs[0]->setTo(num);
						delete num;
					}
					break;
				case OpCodeType::abs:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						if (objs[0]->valueType != OpObjType::Number) {
							this->throwError("tried operating on non number value abs");
						}
						NumberObj* num = static_cast<NumberObj*>(objs[0]->getCopy());
						if (num->value == nullopt) {
							delete num;
							this->throwError("tried operating on null value");
						}
						num->value = abs(*num->value);
						objs[0]->setTo(num);
						delete num;
					}
					break;
				case OpCodeType::min:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
						if (objs[0]->valueType != OpObjType::Number || objs[1]->valueType != OpObjType::Number) {
							this->throwError("tried operating on non number value min");
						}
						NumberObj* a = static_cast<NumberObj*>(objs[0]->getCopy());
						NumberObj* b = static_cast<NumberObj*>(objs[1]->getCopy());
						if (a->value == nullopt || b->value == nullopt) {
							delete a;
							delete b;
							this->throwError("tried operating on null value");
						}
						a->value = fmin(*a->value, *b->value);
						objs[0]->setTo(a);
						delete a;
						delete b;
					}
					break;
				case OpCodeType::max:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
						if (objs[0]->valueType != OpObjType::Number || objs[1]->valueType != OpObjType::Number) {
							this->throwError("tried operating on non number value max");
						}
						NumberObj* a = static_cast<NumberObj*>(objs[0]->getCopy());
						NumberObj* b = static_cast<NumberObj*>(objs[1]->getCopy());
						if (a->value == nullopt || b->value == nullopt) {
							delete a;
							delete b;
							this->throwError("tried operating on null value");
						}
						a->value = fmax(*a->value, *b->value);
						objs[0]->setTo(a);
						delete a;
						delete b;
					}
					break;
				case OpCodeType::clamp:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
						objs[2] = this->linkedObject(&op.obj2, scopes, &freeObjs[2]);
						if (objs[0]->valueType != OpObjType::Number || objs[1]->valueType != OpObjType::Number || objs[2]->valueType != OpObjType::Number) {
							this->throwError("tried operating on non number value clamp");
						}
						NumberObj* v = static_cast<NumberObj*>(objs[0]->getCopy());
						NumberObj* a = static_cast<NumberObj*>(objs[1]->getCopy());
						NumberObj* b = static_cast<NumberObj*>(objs[2]->getCopy());
						if (a->value == nullopt || b->value == nullopt || v->value == nullopt) {
							delete a;
							delete b;
							delete v;
							this->throwError("tried operating on null value");
						}
						v->value = fmin(fmax(*a->value, *v->value), *b->value);
						objs[0]->setTo(v);
						delete a;
						delete b;
						delete v;
					}
					break;
				case OpCodeType::todouble:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						OpObj* val = objs[0]->getCopy();
						NumberObj numObj;
						switch (val->valueType) {
							case OpObjType::String:
								if (static_cast<StringObj*>(val)->value == nullopt) {
									numObj.value == nullopt;
								} else {
									try {
										numObj.value = stod(*static_cast<StringObj*>(val)->value);
									} catch (...) {
										numObj.value = nullopt;
									}
								}
								break;
							case OpObjType::Bool:
								if (static_cast<BoolObj*>(val)->value == nullopt) {
									numObj.value = nullopt;
								} else {
									if (*static_cast<BoolObj*>(val)->value) {
										numObj.value = 1;
									} else {
										numObj.value = 0;
									}
								}
								break;
							default:
								delete val;
								this->throwError("invalid conversion type");
						}
						delete val;
						objs[0]->setTo(&numObj);
					}
					break;
				case OpCodeType::tobool:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						OpObj* val = objs[0]->getCopy();
						BoolObj boolObj;
						switch (val->valueType) {
						case OpObjType::String:
							if (static_cast<StringObj*>(val)->value == nullopt) {
								boolObj.value == nullopt;
							} else {
								if ((*static_cast<StringObj*>(val)->value).length()){
									boolObj.value = true;
								} else {
									boolObj.value = false;
								}
							}
							break;
						case OpObjType::Number:
							if (static_cast<NumberObj*>(val)->value == nullopt) {
								boolObj.value = nullopt;
							} else {
								if (*static_cast<NumberObj*>(val)->value!=0) {
									boolObj.value = 1;
								} else {
									boolObj.value = 0;
								}
							}
							break;
						default:
							delete val;
							this->throwError("invalid conversion type");
						}
						delete val;
						objs[0]->setTo(&boolObj);
					}
					break;
				case OpCodeType::tostring:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
						OpObj* val = objs[0]->getCopy();
						OpObj* digits = objs[1]->getCopy();
						StringObj strObj;
						NumberObj digObj;
						digObj.setTo(digits);
						switch (val->valueType) {
						case OpObjType::Number:
							if (static_cast<NumberObj*>(val)->value == nullopt) {
								strObj.value = "null";
							} else {
								double num = *static_cast<NumberObj*>(val)->value;
								if (digObj.value == nullopt) {
									char strBuff[50];
									snprintf(strBuff, 49, "%g", num);
									strObj.value = strBuff;
								} else {
									int dig = *digObj.value;
									if (dig >= 0) {
										char strBuff[50];
										snprintf(strBuff, 49, "%.*f", dig, num);
										strObj.value = strBuff;
									} else {
										for (int i = 0; i < abs(dig); i++) {
											num = num / 10;
										}
										num = floor(num);
										for (int i = 0; i < abs(dig); i++) {
											num = num * 10;
										}
										num = int(num);
										char strBuff[50];
										snprintf(strBuff, 49, "%.0f", num);
										if (num == 0) {
											strObj.value = "0";
										} else {
											strObj.value = strBuff;
										}
									}
								}
							}
							break;
						case OpObjType::Bool:
							if (static_cast<BoolObj*>(val)->value == nullopt) {
								strObj.value = "null";
							} else {
								if (*static_cast<BoolObj*>(val)->value) {
									strObj.value = "true";
								} else {
									strObj.value = "false";
								}
							}
							break;
						case OpObjType::Null:
							strObj.value = "null";
						default:
							delete val;
							delete digits;
							this->throwError("invalid conversion type");
						}
						delete val;
						delete digits;
						objs[0]->setTo(&strObj);
					}
					break;
				case OpCodeType::len:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						if (objs[0]->valueType != OpObjType::String) {
							this->throwError("tried operating on non string value len");
						}
						StringObj* strObj = static_cast<StringObj*>(objs[0]->getCopy());
						if (strObj->value == nullopt) {
							delete strObj;
							this->throwError("tried to get length of null string");
						}
						NumberObj* numObj = new NumberObj((*strObj->value).length(), false);
						objs[0]->setTo(numObj);
						delete numObj;
						delete strObj;
					}
					break;
					
				case OpCodeType::lcase:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						if (objs[0]->valueType != OpObjType::String) {
							this->throwError("tried operating on non string value lcase");
						}
						StringObj* strObj = static_cast<StringObj*>(objs[0]->getCopy());
						if (strObj->value == nullopt) {
							delete strObj;
							this->throwError("tried to set null to lowercase");
						}
						transform((*strObj->value).begin(), (*strObj->value).end(), (*strObj->value).begin(), [](unsigned char c) { return std::tolower(c); });
						objs[0]->setTo(strObj);
						delete strObj;
					}
					break;
				case OpCodeType::ucase:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						if (objs[0]->valueType != OpObjType::String) {
							this->throwError("tried operating on non string value ucase");
						}
						StringObj* strObj = static_cast<StringObj*>(objs[0]->getCopy());
						if (strObj->value == nullopt) {
							delete strObj;
							this->throwError("tried to set null to uppercase");
						}
						transform((*strObj->value).begin(), (*strObj->value).end(), (*strObj->value).begin(), [](unsigned char c) { return std::toupper(c); });
						objs[0]->setTo(strObj);
						delete strObj;
					}
					break;
				case OpCodeType::concat:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
						if (objs[0]->valueType != OpObjType::String || objs[1]->valueType != OpObjType::String) {
							this->throwError("tried operating on non string value concat");
						}
						StringObj* strObj = static_cast<StringObj*>(objs[0]->getCopy());
						StringObj* str2Obj = static_cast<StringObj*>(objs[1]->getCopy());
						if (strObj->value == nullopt || str2Obj->value==nullopt) {
							delete strObj;
							delete str2Obj;
							this->throwError("tried to concat with null");
						}
						strObj->value = *strObj->value + *str2Obj->value;
						objs[0]->setTo(strObj);
						delete strObj;
						delete str2Obj;
					}
					break;
				case OpCodeType::trim:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						if (objs[0]->valueType != OpObjType::String) {
							this->throwError("tried operating on non string value trim");
						}
						StringObj* strObj = static_cast<StringObj*>(objs[0]->getCopy());
						if (strObj->value == nullopt) {
							delete strObj;
							this->throwError("tried to trim null");
						}
						string str = *strObj->value;
						trim(str);
						strObj->value = str;
						objs[0]->setTo(strObj);
						delete strObj;
					}
					break;
				case OpCodeType::substr:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
						objs[2] = this->linkedObject(&op.obj2, scopes, &freeObjs[2]);
						if (objs[0]->valueType != OpObjType::String || objs[1]->valueType != OpObjType::Number || objs[2]->valueType!=OpObjType::Number) {
							this->throwError("tried operating on bad types substr");
						}
						StringObj* strObj = static_cast<StringObj*>(objs[0]->getCopy());
						NumberObj* startObj = static_cast<NumberObj*>(objs[1]->getCopy());
						NumberObj* lenObj = static_cast<NumberObj*>(objs[2]->getCopy());
						if (strObj->value == nullopt && startObj->value == nullopt && lenObj->value == nullopt) {
							delete strObj;
							delete startObj;
							delete lenObj;
							this->throwError("tried to get substring with null param");
						}
						if (*startObj->value >= (*strObj->value).length()) {
							StringObj *empty = new StringObj("", false);
							objs[0]->setTo(empty);
							delete empty;
						} else {
							int start = *startObj->value;
							int length = *lenObj->value;
							if (start < 0) start = (*strObj->value).length() + start;
							StringObj* substrObj = new StringObj((*strObj->value).substr(start, length), false);
							objs[0]->setTo(substrObj);
							delete substrObj;
						}
						delete strObj;
						delete startObj;
						delete lenObj;
					}
					break;
				case OpCodeType::mov:
					objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
					objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
					objs[0]->setTo(objs[1]);
					break;
				case OpCodeType::notOp:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						BoolObj* boolObj = static_cast<BoolObj*>(objs[0]->getCopy());
						if (objs[0]->valueType != OpObjType::Bool ) {
							this->throwError("tried operating on non bool value not");
						}
						if (boolObj->value == nullopt) {
							delete boolObj;
							this->throwError("tried operating on null value not");
						}
						boolObj->value = ! *boolObj->value;
						objs[0]->setTo(boolObj);
						delete boolObj;
					}
					break;
				case OpCodeType::andOp:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
						if (objs[0]->valueType != OpObjType::Bool || objs[1]->valueType != OpObjType::Bool) {
							this->throwError("tried operating on non bool value and");
						}
						BoolObj* a = static_cast<BoolObj*>(objs[0]->getCopy());
						BoolObj* b = static_cast<BoolObj*>(objs[1]->getCopy());
						if (a->value == nullopt || b->value == nullopt) {
							delete a;
							delete b;
							this->throwError("tried operating on null value and");
						}
						a->value = *a->value && *b->value;
						objs[0]->setTo(a);
						delete a;
						delete b;
					}
					break;
				case OpCodeType::orOp:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
						if (objs[0]->valueType != OpObjType::Bool || objs[1]->valueType != OpObjType::Bool) {
							this->throwError("tried operating on non bool value or");
						}
						BoolObj* a = static_cast<BoolObj*>(objs[0]->getCopy());
						BoolObj* b = static_cast<BoolObj*>(objs[1]->getCopy());
						if (a->value == nullopt || b->value == nullopt) {
							delete a;
							delete b;
							this->throwError("tried operating on null value or");
						}
						a->value = *a->value || *b->value;
						objs[0]->setTo(a);
						delete a;
						delete b;
					}
					break;
				case OpCodeType::add:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
						if (objs[0]->valueType != OpObjType::Number || objs[1]->valueType != OpObjType::Number) {
							this->throwError("tried operating on non number value add");
						}
						NumberObj* a = static_cast<NumberObj*>(objs[0]->getCopy());
						NumberObj* b = static_cast<NumberObj*>(objs[1]->getCopy());
						if (a->value == nullopt || b->value == nullopt) {
							delete a;
							delete b;
							this->throwError("tried operating on null value add");
						}
						a->value = *a->value + *b->value;
						objs[0]->setTo(a);
						delete a;
						delete b;
					}
					break;
				case OpCodeType::sub:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
						if (objs[0]->valueType != OpObjType::Number || objs[1]->valueType != OpObjType::Number) {
							this->throwError("tried operating on non number value sub");
						}
						NumberObj* a = static_cast<NumberObj*>(objs[0]->getCopy());
						NumberObj* b = static_cast<NumberObj*>(objs[1]->getCopy());
						if (a->value == nullopt || b->value == nullopt) {
							delete a;
							delete b;
							this->throwError("tried operating on null value sub");
						}
						a->value = *a->value - *b->value;
						objs[0]->setTo(a);
						delete a;
						delete b;
					}
					break;
				case OpCodeType::mul:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
						if (objs[0]->valueType != OpObjType::Number || objs[1]->valueType != OpObjType::Number) {
							this->throwError("tried operating on non number value mul");
						}
						NumberObj* a = static_cast<NumberObj*>(objs[0]->getCopy());
						NumberObj* b = static_cast<NumberObj*>(objs[1]->getCopy());
						if (a->value == nullopt || b->value == nullopt) {
							delete a;
							delete b;
							this->throwError("tried operating on null value mul");
						}
						a->value = *a->value * *b->value;
						objs[0]->setTo(a);
						delete a;
						delete b;
					}
					break;
				case OpCodeType::div:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
						if (objs[0]->valueType != OpObjType::Number || objs[1]->valueType != OpObjType::Number) {
							this->throwError("tried operating on non number value div");
						}
						NumberObj* a = static_cast<NumberObj*>(objs[0]->getCopy());
						NumberObj* b = static_cast<NumberObj*>(objs[1]->getCopy());
						if (a->value == nullopt || b->value == nullopt) {
							delete a;
							delete b;
							this->throwError("tried operating on null value div");
						}
						a->value = *a->value / *b->value;
						objs[0]->setTo(a);
						delete a;
						delete b;
					}
					break;
				case OpCodeType::mod:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
						if (objs[0]->valueType != OpObjType::Number || objs[1]->valueType != OpObjType::Number) {
							this->throwError("tried operating on non number value %");
						}
						NumberObj* a = static_cast<NumberObj*>(objs[0]->getCopy());
						NumberObj* b = static_cast<NumberObj*>(objs[1]->getCopy());
						if (a->value == nullopt || b->value == nullopt) {
							delete a;
							delete b;
							this->throwError("tried operating on null value %");
						}
						a->value = fmod(*a->value , *b->value);
						objs[0]->setTo(a);
						delete a;
						delete b;
					}
					break;
				case OpCodeType::exponent:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						objs[1] = this->linkedObject(&op.obj1, scopes, &freeObjs[1]);
						if (objs[0]->valueType != OpObjType::Number || objs[1]->valueType != OpObjType::Number) {
							this->throwError("tried operating on non number value ^");
						}
						NumberObj* a = static_cast<NumberObj*>(objs[0]->getCopy());
						NumberObj* b = static_cast<NumberObj*>(objs[1]->getCopy());
						if (a->value == nullopt || b->value == nullopt) {
							delete a;
							delete b;
							this->throwError("tried operating on null value ^");
						}
						a->value = pow(*a->value, *b->value);
						objs[0]->setTo(a);
						delete a;
						delete b;
					}
					break;
				case OpCodeType::neg:
					{
						objs[0] = this->linkedObject(&op.obj0, scopes, &freeObjs[0]);
						if (objs[0]->valueType != OpObjType::Number) {
							this->throwError("tried operating on non number value neg");
						}
						NumberObj* num = static_cast<NumberObj*>(objs[0]->getCopy());
						if (num->value == nullopt) {
							delete num;
							this->throwError("tried operating on null value neg");
						}
						num->value = *num->value * -1;
						objs[0]->setTo(num);
						delete num;
					}
					break;
				case OpCodeType::scopeDepth:
					for (size_t i = 0; i < op.size; i++) {
						scopes.push_back({{}});
					}
					break;
				default:
					this->throwError("unknown opcode type");
			}
			if (freeObjs[0] && objs[0]) { delete objs[0]; objs[0] = nullptr; freeObjs[0] = false; }
			if (freeObjs[1] && objs[1]) { delete objs[1]; objs[1] = nullptr; freeObjs[1] = false; }
			if (freeObjs[2] && objs[2]) { delete objs[2]; objs[2] = nullptr; freeObjs[2] = false; }
			eip++;
		}
	} catch (char e) {
		cleanUp(scopes, cleanUpStack, stack, objs, freeObjs);
		throw 'E';
	} catch (const char* e) {
		this->errorMsg = "Execution OpObj Error: " + string(e);
		cleanUp(scopes, cleanUpStack, stack, objs, freeObjs);
		throw 'E';
	}

	cleanUp(scopes, cleanUpStack, stack, objs, freeObjs);

	return nullptr;
}