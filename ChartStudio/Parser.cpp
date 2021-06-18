#include "stdafx.h"
#include "OpCode.h"
#include "Parser.h"
#include <algorithm>

string IdentityTypeToString(IdentityType type) {
	switch (type) {
	case IdentityType::Null:
		return string("null");

	case IdentityType::Bool:
		return string("Bool");

	case IdentityType::Double:
		return string("Double");

	case IdentityType::String:
		return string("String");

	case IdentityType::Function:
		return string("Function");
	}
	return string("Unknown");
}

string TokenTypeToString(InterpreterTokenType type) {
	switch (type) {
		case InterpreterTokenType::LineDelim:
			return string(";");
		case InterpreterTokenType::EndOfFile:
			return string("end of file");
		case InterpreterTokenType::NewLine:
			return string("new line");
		case InterpreterTokenType::Void:
			return string("void");
		case InterpreterTokenType::Double:
			return string("double");
		case InterpreterTokenType::String:
			return string("string");
		case InterpreterTokenType::Bool:
			return string("bool");
		case InterpreterTokenType::DoubleLiteral:
			return string("double literal");
		case InterpreterTokenType::StringLiteral:
			return string("string literal");
		case InterpreterTokenType::BoolLiteral:
			return string("bool literal");
		case InterpreterTokenType::Ident:
			return string("ident");
		case InterpreterTokenType::True:
			return string("true");
		case InterpreterTokenType::False:
			return string("false");
		case InterpreterTokenType::Null:
			return string("null");
		case InterpreterTokenType::LeftParen:
			return string("(");
		case InterpreterTokenType::RightParen:
			return string(")");
		case InterpreterTokenType::LeftSquare:
			return string("[");
		case InterpreterTokenType::RightSquare:
			return string("]");
		case InterpreterTokenType::Comma:
			return string(",");
		case InterpreterTokenType::Dot:
			return string(".");
		case InterpreterTokenType::Not:
			return string("!");
		case InterpreterTokenType::And:
			return string("&&");
		case InterpreterTokenType::Or:
			return string("||");
		case InterpreterTokenType::Plus:
			return string("+");
		case InterpreterTokenType::Minus:
			return string("-");
		case InterpreterTokenType::Divide:
			return string("/");
		case InterpreterTokenType::Multiply:
			return string("*");
		case InterpreterTokenType::Mod:
			return string("%");
		case InterpreterTokenType::Exponent:
			return string("^");
		case InterpreterTokenType::Question:
			return string("?");
		case InterpreterTokenType::Colon:
			return string(":");
		case InterpreterTokenType::Assignment:
			return string("=");
		case InterpreterTokenType::Equals:
			return string("==");
		case InterpreterTokenType::NotEquals:
			return string("!=");
		case InterpreterTokenType::Lesser:
			return string("<");
		case InterpreterTokenType::Greater:
			return string(">");
		case InterpreterTokenType::LesserEquals:
			return string("<=");
		case InterpreterTokenType::GreaterEquals:
			return string(">=");
		case InterpreterTokenType::Min:
			return string("min");
		case InterpreterTokenType::Max:
			return string("max");
		case InterpreterTokenType::Abs:
			return string("abs");
		case InterpreterTokenType::Clamp:
			return string("clamp");
		case InterpreterTokenType::Floor:
			return string("floor");
		case InterpreterTokenType::Ceil:
			return string("ceil");
		case InterpreterTokenType::LCase:
			return string("lcase");
		case InterpreterTokenType::UCase:
			return string("ucase");
		case InterpreterTokenType::Trim:
			return string("trim");
		case InterpreterTokenType::Len:
			return string("len");
		case InterpreterTokenType::SubStr:
			return string("substr");
		case InterpreterTokenType::While:
			return string("while");
		case InterpreterTokenType::For:
			return string("for");
		case InterpreterTokenType::Loop:
			return string("loop");
		case InterpreterTokenType::If:
			return string("if");
		case InterpreterTokenType::Else:
			return string("else");
		case InterpreterTokenType::Break:
			return string("break");
		case InterpreterTokenType::LeftCurly:
			return string("{");
		case InterpreterTokenType::RightCurly:
			return string("}");
		case InterpreterTokenType::Return:
			return string("return");
		case InterpreterTokenType::Exit:
			return string("exit");
	}
	return string("Unknown");
}

Parser::~Parser() {
}

Parser::Parser(vector<TokenObj>& tokens) {
	this->errorMsg = "";
	this->tokens = tokens;

	this->tokenIndex = 0;
	this->token = &this->tokens[0];
	this->tokenEndIndex = this->tokens.size();

	this->branchCounter = 1;

	this->scopeIndex = 0;
	this->scopes.clear();
	this->scopes.push_back({});

	this->allocScope.clear();
	this->allocScope.push_back(0);
	this->allocScopeIndex = 0;

	this->externalsScopeIndex = 0;

	this->maxScopeDepth = 0;
}

size_t Parser::newBranch() {
	return this->branchCounter++;
}

void Parser::throwError(string message) {
	size_t errorLine = 0;
	if (this->token) {
		errorLine = this->token->line;
	} else {
		errorLine = this->tokens[this->tokens.size() - 1].line;
	}
	this->errorMsg = "Parser error on line " + to_string(errorLine) + ": " + message;
	throw 'P';
}

void Parser::typeMismatch(IdentityType expectedType, IdentityType foundType) {
	this->throwError("expected type " + IdentityTypeToString(expectedType) + " but found type " + IdentityTypeToString(foundType));
}

void Parser::matchType(IdentityType whatToTest, IdentityType typeItShouldBe, bool strict = false) {
	if (this->typesDontMatch(whatToTest, typeItShouldBe, strict))  this->typeMismatch(typeItShouldBe, whatToTest);
}

bool Parser::typesDontMatch(IdentityType a, IdentityType b, bool strict = false) {
	return !(a==b || ((a==IdentityType::Null || b==IdentityType::Null) && strict==false));
}

void Parser::assertType(IdentityType whatItIs, IdentityType whatItShouldBe, bool strict = false) {
	if (this->typesDontMatch(whatItIs, whatItShouldBe, strict)) this->typeMismatch(whatItIs, whatItShouldBe);
}

void Parser::match(InterpreterTokenType type) {
	if (this->token && this->token->type == type) {
		this->getToken();
	} else {
		if (this->token) {
			this->throwError("expected token type " + TokenTypeToString(type) + " but found " + TokenTypeToString(this->token->type) + " instead");
		} else {
			this->throwError("expected token type " + TokenTypeToString(type) + " but found nothing!");
		}
	}
}

bool Parser::isNotEnd() {
	return this->tokenIndex < this->tokenEndIndex;
}

void Parser::getToken() {
	if (this->isNotEnd()) {
		this->tokenIndex++;
		if (!this->isNotEnd()) return;
		this->token = &this->tokens[this->tokenIndex];

		while (this->token && this->token->type == InterpreterTokenType::NewLine) {
			//this->program.addCode(OpCode::codeLine(this->token->sValue));
			this->tokenIndex++;
			if (!this->isNotEnd()) break;
			this->token = &this->tokens[this->tokenIndex];
		}
	}
}

void Parser::firstToken() {
	this->tokenIndex = 0;
	this->token = &this->tokens[0];

	while (this->token && this->token->type == InterpreterTokenType::NewLine) {
		this->tokenIndex++;
		this->token = &this->tokens[this->tokenIndex];
	}
}

void Parser::parse(vector<ExternalDef> externals, optional<IdentityType> expectExitType) {
	this->exitType = expectExitType;
	
	this->tokenIndex = this->tokens.size();
	this->firstToken();
	

	this->branchCounter = 1;

	this->externalsScopeIndex = 0;
	this->scopeIndex = 0;
	this->scopes.clear();
	this->scopes.push_back({});

	this->allocScope.clear();
	this->allocScope.push_back(0);
	this->maxScopeDepth = 0;

	this->program.reset();

	for (size_t i = 0; i < externals.size(); i++) {
		this->addToCurrentScope(externals[i].name, externals[i].type, i, externals[i].params, externals[i].returnType);
	}

	this->pushAllocScope();

	const size_t entryPoint = this->newBranch();
	const size_t entryPointAddress = this->program.addCode(OpCode::label(entryPoint));

	this->doBlock(nullopt, nullopt, false, false, true, nullopt);
	this->program.addCode( OpCode::exit(UnlinkedObj()) );

	vector<OpCode> mainPreamble = {OpCode::scopeDepth(this->maxScopeDepth)};
	if (this->allocScope[this->allocScopeIndex]) {
		mainPreamble.push_back(OpCode::pushScope(this->allocScopeIndex, this->allocScope[this->allocScopeIndex]));
	}
	this->program.insertCode(mainPreamble, entryPointAddress);

	this->popAllocScope();

	this->popScope();
}

void Parser::pushAllocScope() {
	this->allocScopeIndex++;
	this->allocScope.push_back(0);
	this->maxScopeDepth = max(this->maxScopeDepth, this->allocScopeIndex);
}

void Parser::popAllocScope() {
	this->allocScope.pop_back();
	this->allocScopeIndex--;
}

void Parser::pushScope() {
	this->scopes.push_back({});
	this->scopeIndex++;
}

void Parser::popScope() {
	this->scopes.pop_back();
	this->scopeIndex--;
}


ScopeObj Parser::addToCurrentScope(string name, IdentityType type, size_t branch = 0, const vector<IdentityType> params = {}, IdentityType returnType = IdentityType::Null) {
	bool alreadyExists = this->getIdentity(name, true) != nullptr;
	if (alreadyExists) this->throwError("duplicate name definition, " + name + " already exists in current scope");

	ScopeObj obj = ScopeObj(name, type, branch, params, returnType, this->allocScopeIndex, (this->allocScopeIndex==0 ? this->externalsScopeIndex : this->allocScope[this->allocScopeIndex]) );

	this->scopes[this->scopeIndex].push_back(obj);
	if (this->allocScopeIndex == 0) {
		this->externalsScopeIndex++;
	}
	switch (type) {
		case IdentityType::Bool:
		case IdentityType::Double:
		case IdentityType::String:
			this->allocScope[this->allocScopeIndex]++;
			break;
	}

	return obj;
}


ScopeObj* Parser::getIdentity(string name, bool onlyInCurrentScope=false) {
	if (this->scopes.size() == 0) return nullptr;
	for (int i = this->scopeIndex;i>=0;i--){
		if (this->scopes[i].size() > 0) {
			for (size_t j = 0; j < this->scopes[i].size(); j++) {
				if (this->scopes[i][j].name == name) return &this->scopes[i][j];
			}
		}
		if (onlyInCurrentScope) break;
	}
	return nullptr;
}

ScopeObj Parser::addVar(string name, IdentityType type) {
	return this->addToCurrentScope(name, type);
}

ScopeObj Parser::addFunction(string name, IdentityType returnType, size_t branch, vector<IdentityType> params) {
	return this->addToCurrentScope(name, IdentityType::Function, branch, params, returnType);
}


bool Parser::isPowerOp() {
	return this->token->type == InterpreterTokenType::Exponent;
}

bool Parser::isTermOp() {
	switch (this->token->type) {
		case InterpreterTokenType::Multiply:
		case InterpreterTokenType::Divide:
		case InterpreterTokenType::Mod:
			return true;
	}
	return false;
}

bool Parser::isAddOp() {
	switch (this->token->type) {
	case InterpreterTokenType::Plus:
	case InterpreterTokenType::Minus:
		return true;
	}
	return false;
}

bool Parser::isCompareOp() {
	switch (this->token->type) {
	case InterpreterTokenType::Lesser:
	case InterpreterTokenType::LesserEquals:
	case InterpreterTokenType::Greater:
	case InterpreterTokenType::GreaterEquals:
	case InterpreterTokenType::Equals:
	case InterpreterTokenType::NotEquals:
		return true;
	}
	return false;
}

bool Parser::isOrOp() {
	return this->token->type == InterpreterTokenType::Or;
}

bool Parser::isAndOp() {
	return this->token->type == InterpreterTokenType::And;
}

bool Parser::isTernaryOp() {
	return this->token->type == InterpreterTokenType::Question;
}

IdentityType Parser::doFuncCall(string funcName = "") {
	bool needsIdentMatched = false;
	if (funcName.empty()) {
		funcName = this->token->sValue;
		needsIdentMatched = true;
	}

	ScopeObj* identObj = this->getIdentity(funcName);
	if (!identObj) this->throwError("tried to call undefined function " + funcName);
	if (identObj->type != IdentityType::Function) this->throwError("tried to call undefined function '" + funcName + "'");


	if (needsIdentMatched) this->match(InterpreterTokenType::Ident);
	this->match(InterpreterTokenType::LeftParen);

	for (size_t i = 0; i < identObj->params.size(); i++) {
		IdentityType type = this->doExpression();
		this->program.addCode(OpCode::push(UnlinkedObj(RegisterId::eax)));

		this->assertType(type, identObj->params[i]);

		if (i < identObj->params.size() - 1) {
			this->match(InterpreterTokenType::Comma);
		}
	}

	this->match(InterpreterTokenType::RightParen);

	if (identObj->scope == 0) {
		this->program.addCode(OpCode::exCall(identObj->branch));
	} else {
		this->program.addCode(OpCode::call(identObj->branch));
	}

	return identObj->returnType;
}

IdentityType Parser::doIdent() {
	string varName = this->token->sValue;
	ScopeObj* identObj = this->getIdentity(varName);
	this->match(InterpreterTokenType::Ident);
	if (!identObj) this->throwError("tried to access undefined '" + varName + "'");
	
	switch (identObj->type) {
	case IdentityType::Function:
		return this->doFuncCall(varName);

	case IdentityType::String:
	case IdentityType::Double:
	case IdentityType::Bool:
		this->program.addCode(OpCode::mov(UnlinkedObj(RegisterId::eax), UnlinkedObj(identObj->type, identObj->scope, identObj->index)));
		return identObj->type;
	}

	this->throwError("unknown ident type for '" + varName+"'");
	return IdentityType::Null;
}

IdentityType Parser::doFactor() {
	IdentityType type = IdentityType::Null;
	switch (this->token->type) {
		case InterpreterTokenType::Ident:
			return this->doIdent();

		case InterpreterTokenType::Minus:
			this->match(InterpreterTokenType::Minus);
			type = this->doFactor();
			this->assertType(type, IdentityType::Double);
			this->program.addCode(OpCode::neg( UnlinkedObj(RegisterId::eax) ));
			return IdentityType::Double;

		case InterpreterTokenType::Not:
			this->match(InterpreterTokenType::Not);
			type = this->doFactor();
			this->assertType(type, IdentityType::Bool);
			this->program.addCode(OpCode::notOp( UnlinkedObj(RegisterId::eax) ));
			return IdentityType::Bool;

		case InterpreterTokenType::Question:
			this->match(InterpreterTokenType::Question);
			type = this->doFactor();
			this->program.addCode(OpCode::cmp( UnlinkedObj(RegisterId::eax), UnlinkedObj() ));
			this->program.addCode(OpCode::se( UnlinkedObj(RegisterId::eax) ));
			return IdentityType::Bool;

		case InterpreterTokenType::LeftParen:
			this->match(InterpreterTokenType::LeftParen);
			type = this->doExpression();
			this->match(InterpreterTokenType::RightParen);
			return type;

		case InterpreterTokenType::Null:
			this->match(InterpreterTokenType::Null);
			this->program.addCode(OpCode::mov( UnlinkedObj(RegisterId::eax), UnlinkedObj() ));
			return IdentityType::Null;

		case InterpreterTokenType::True:
			this->match(InterpreterTokenType::True);
			this->program.addCode(OpCode::mov(UnlinkedObj(RegisterId::eax), UnlinkedObj(true)));
			return IdentityType::Bool;

		case InterpreterTokenType::False:
			this->match(InterpreterTokenType::False);
			this->program.addCode(OpCode::mov(UnlinkedObj(RegisterId::eax), UnlinkedObj(false)));
			return IdentityType::Bool;

		case InterpreterTokenType::DoubleLiteral:
			this->program.addCode(OpCode::mov( UnlinkedObj(RegisterId::eax), UnlinkedObj(this->token->dValue) ));
			this->match(InterpreterTokenType::DoubleLiteral);
			return IdentityType::Double;

		case InterpreterTokenType::StringLiteral:
			this->program.addCode(OpCode::mov(UnlinkedObj(RegisterId::eax), UnlinkedObj(this->token->sValue)));
			this->match(InterpreterTokenType::StringLiteral);
			return IdentityType::String;

		case InterpreterTokenType::Bool:
			this->match(InterpreterTokenType::Bool);
			this->match(InterpreterTokenType::LeftParen);
			type = this->doExpression();
			if (this->typesDontMatch(IdentityType::Bool, type)) {
				this->program.addCode( OpCode::toBool(UnlinkedObj(RegisterId::eax)) );
			}
			this->match(InterpreterTokenType::RightParen);
			return IdentityType::Bool;

		case InterpreterTokenType::Double:
			this->match(InterpreterTokenType::Double);
			this->match(InterpreterTokenType::LeftParen);
			type = this->doExpression();
			if (this->typesDontMatch(IdentityType::Double, type)) {
				this->program.addCode(OpCode::toDouble(UnlinkedObj(RegisterId::eax)));
			}
			this->match(InterpreterTokenType::RightParen);
			return IdentityType::Double;

		case InterpreterTokenType::String:
			this->match(InterpreterTokenType::String);
			this->match(InterpreterTokenType::LeftParen);
			type = this->doExpression();
			if (this->token->type == InterpreterTokenType::Comma) {
				if (!this->typesDontMatch(type, IdentityType::String)) this->throwError("cannot do string:decimals conversion on a string");
				this->match(InterpreterTokenType::Comma);
				this->program.addCode(OpCode::push( UnlinkedObj(RegisterId::eax) ));
				this->assertType(this->doExpression(), IdentityType::Double);
				this->program.addCode(OpCode::pop(UnlinkedObj(RegisterId::ebx)));
				this->program.addCode(OpCode::toString(UnlinkedObj(RegisterId::ebx), UnlinkedObj(RegisterId::eax)));
				this->program.addCode(OpCode::mov(UnlinkedObj(RegisterId::eax), UnlinkedObj(RegisterId::ebx)));
			} else {
				if (this->typesDontMatch(type, IdentityType::String)) {
					this->program.addCode(OpCode::toString(UnlinkedObj(RegisterId::eax), UnlinkedObj()));
				}
			}
			this->match(InterpreterTokenType::RightParen);
			return IdentityType::String;

		case InterpreterTokenType::Ceil:
			this->match(InterpreterTokenType::Ceil);
			this->match(InterpreterTokenType::LeftParen);
			this->assertType(this->doExpression(), IdentityType::Double);
			this->program.addCode(OpCode::ceil(UnlinkedObj(RegisterId::eax)));
			this->match(InterpreterTokenType::RightParen);
			return IdentityType::Double;

		case InterpreterTokenType::Floor:
			this->match(InterpreterTokenType::Floor);
			this->match(InterpreterTokenType::LeftParen);
			this->assertType(this->doExpression(), IdentityType::Double);
			this->program.addCode(OpCode::floor(UnlinkedObj(RegisterId::eax)));
			this->match(InterpreterTokenType::RightParen);
			return IdentityType::Double;

		case InterpreterTokenType::Abs:
			this->match(InterpreterTokenType::Abs);
			this->match(InterpreterTokenType::LeftParen);
			this->assertType(this->doExpression(), IdentityType::Double);
			this->program.addCode(OpCode::abs(UnlinkedObj(RegisterId::eax)));
			this->match(InterpreterTokenType::RightParen);
			return IdentityType::Double;

		case InterpreterTokenType::Min:
			this->match(InterpreterTokenType::Min);
			this->match(InterpreterTokenType::LeftParen);
			this->assertType(this->doExpression(), IdentityType::Double);
			this->program.addCode(OpCode::push(UnlinkedObj(RegisterId::eax)));
			this->match(InterpreterTokenType::Comma);
			this->assertType(this->doExpression(), IdentityType::Double);
			this->program.addCode(OpCode::pop(UnlinkedObj(RegisterId::ebx)));
			this->program.addCode(OpCode::minOp(UnlinkedObj(RegisterId::eax), UnlinkedObj(RegisterId::ebx)));
			this->match(InterpreterTokenType::RightParen);
			return IdentityType::Double;

		case InterpreterTokenType::Max:
			this->match(InterpreterTokenType::Max);
			this->match(InterpreterTokenType::LeftParen);
			this->assertType(this->doExpression(), IdentityType::Double);
			this->program.addCode(OpCode::push(UnlinkedObj(RegisterId::eax)));
			this->match(InterpreterTokenType::Comma);
			this->assertType(this->doExpression(), IdentityType::Double);
			this->program.addCode(OpCode::pop(UnlinkedObj(RegisterId::ebx)));
			this->program.addCode(OpCode::maxOp(UnlinkedObj(RegisterId::eax), UnlinkedObj(RegisterId::ebx)));
			this->match(InterpreterTokenType::RightParen);
			return IdentityType::Double;

		case InterpreterTokenType::Clamp:
			this->match(InterpreterTokenType::Clamp);
			this->match(InterpreterTokenType::LeftParen);
			this->assertType(this->doExpression(), IdentityType::Double);
			this->program.addCode(OpCode::push(UnlinkedObj(RegisterId::eax)));
			this->match(InterpreterTokenType::Comma);
			this->assertType(this->doExpression(), IdentityType::Double);
			this->program.addCode(OpCode::pop(UnlinkedObj(RegisterId::ebx)));
			this->program.addCode(OpCode::maxOp(UnlinkedObj(RegisterId::eax), UnlinkedObj(RegisterId::ebx)));
			this->program.addCode(OpCode::push(UnlinkedObj(RegisterId::eax)));
			this->match(InterpreterTokenType::Comma);
			this->assertType(this->doExpression(), IdentityType::Double);
			this->program.addCode(OpCode::pop(UnlinkedObj(RegisterId::ebx)));
			this->program.addCode(OpCode::minOp(UnlinkedObj(RegisterId::eax), UnlinkedObj(RegisterId::ebx)));
			this->match(InterpreterTokenType::RightParen);
			return IdentityType::Double;

		case InterpreterTokenType::Len:
			this->match(InterpreterTokenType::Len);
			this->match(InterpreterTokenType::LeftParen);
			this->assertType(this->doExpression(), IdentityType::String);
			this->program.addCode(OpCode::len(UnlinkedObj(RegisterId::eax)));
			this->match(InterpreterTokenType::RightParen);
			return IdentityType::Double;

		case InterpreterTokenType::SubStr:
			this->match(InterpreterTokenType::SubStr);
			this->match(InterpreterTokenType::LeftParen);
			this->assertType(this->doExpression(), IdentityType::String);
			this->program.addCode(OpCode::push(UnlinkedObj(RegisterId::eax)));
			this->match(InterpreterTokenType::Comma);
			this->assertType(this->doExpression(), IdentityType::Double);
			this->program.addCode(OpCode::push(UnlinkedObj(RegisterId::eax)));
			this->match(InterpreterTokenType::Comma);
			this->assertType(this->doExpression(), IdentityType::Double);
			this->program.addCode(OpCode::mov(UnlinkedObj(RegisterId::ecx), UnlinkedObj(RegisterId::eax)));
			this->program.addCode(OpCode::pop(UnlinkedObj(RegisterId::ebx)));
			this->program.addCode(OpCode::pop(UnlinkedObj(RegisterId::eax)));
			this->program.addCode(OpCode::subStr(UnlinkedObj(RegisterId::eax), UnlinkedObj(RegisterId::ebx), UnlinkedObj(RegisterId::ecx)));
			this->match(InterpreterTokenType::RightParen);
			return IdentityType::String;

		case InterpreterTokenType::Trim:
			this->match(InterpreterTokenType::Trim);
			this->match(InterpreterTokenType::LeftParen);
			this->assertType(this->doExpression(), IdentityType::String);
			this->program.addCode(OpCode::trim(UnlinkedObj(RegisterId::eax)));
			this->match(InterpreterTokenType::RightParen);
			return IdentityType::String;

		case InterpreterTokenType::UCase:
			this->match(InterpreterTokenType::UCase);
			this->match(InterpreterTokenType::LeftParen);
			this->assertType(this->doExpression(), IdentityType::String);
			this->program.addCode(OpCode::ucase(UnlinkedObj(RegisterId::eax)));
			this->match(InterpreterTokenType::RightParen);
			return IdentityType::String;

		case InterpreterTokenType::LCase:
			this->match(InterpreterTokenType::LCase);
			this->match(InterpreterTokenType::LeftParen);
			this->assertType(this->doExpression(), IdentityType::String);
			this->program.addCode(OpCode::lcase(UnlinkedObj(RegisterId::eax)));
			this->match(InterpreterTokenType::RightParen);
			return IdentityType::String;
		default:
			this->throwError("expected factor but found "+ TokenTypeToString(this->token->type));
	}
	return IdentityType::Null;
}

IdentityType Parser::doExponentiation() {
	IdentityType leftType = this->doFactor();

	while (this->isNotEnd() && this->isPowerOp()) {
		this->program.addCode(OpCode::push(UnlinkedObj(RegisterId::eax)));

		InterpreterTokenType powerOp = this->token->type;
		this->match(powerOp);
		this->assertType(leftType, IdentityType::Double);
		this->assertType(this->doFactor(), IdentityType::Double);

		this->program.addCode(OpCode::pop(UnlinkedObj(RegisterId::ebx)));

		switch (powerOp) {
			case InterpreterTokenType::Exponent:
				this->program.addCode(OpCode::exponent( UnlinkedObj(RegisterId::ebx), UnlinkedObj(RegisterId::eax) ));
				this->program.addCode(OpCode::mov( UnlinkedObj(RegisterId::eax), UnlinkedObj(RegisterId::ebx) ));
				break;
		}
	}

	return leftType;
}


IdentityType Parser::doTerm() {
	IdentityType leftType = this->doExponentiation();

	while (this->isNotEnd() && this->isTermOp()) {
		this->program.addCode(OpCode::push(UnlinkedObj(RegisterId::eax)));

		InterpreterTokenType termOp = this->token->type;
		this->match(termOp);
		this->assertType(leftType, IdentityType::Double);
		this->assertType(this->doExponentiation(), IdentityType::Double);

		this->program.addCode(OpCode::pop(UnlinkedObj(RegisterId::ebx)));

		switch (termOp) {
		case InterpreterTokenType::Multiply:
			this->program.addCode(OpCode::mul(UnlinkedObj(RegisterId::eax), UnlinkedObj(RegisterId::ebx)));
			break;
		case InterpreterTokenType::Divide:
			this->program.addCode(OpCode::div(UnlinkedObj(RegisterId::ebx), UnlinkedObj(RegisterId::eax)));
			this->program.addCode(OpCode::mov(UnlinkedObj(RegisterId::eax), UnlinkedObj(RegisterId::ebx)));
			break;
		case InterpreterTokenType::Mod:
			this->program.addCode(OpCode::mod(UnlinkedObj(RegisterId::ebx), UnlinkedObj(RegisterId::eax)));
			this->program.addCode(OpCode::mov(UnlinkedObj(RegisterId::eax), UnlinkedObj(RegisterId::ebx)));
			break;
		}
	}

	return leftType;
}

IdentityType Parser::doAdd() {
	IdentityType leftType = this->doTerm();

	while (this->isNotEnd() && this->isAddOp()) {
		this->program.addCode(OpCode::push(UnlinkedObj(RegisterId::eax)));

		InterpreterTokenType addOp = this->token->type;
		this->match(addOp);
		this->assertType(this->doTerm(), leftType);

		this->program.addCode(OpCode::pop(UnlinkedObj(RegisterId::ebx)));

		switch (addOp) {
		case InterpreterTokenType::Plus:
			if (leftType == IdentityType::String) {
				this->program.addCode(OpCode::concat(UnlinkedObj(RegisterId::ebx), UnlinkedObj(RegisterId::eax)));
				this->program.addCode(OpCode::mov(UnlinkedObj(RegisterId::eax), UnlinkedObj(RegisterId::ebx)));
			} else {
				this->assertType(leftType, IdentityType::Double);
				this->program.addCode(OpCode::add(UnlinkedObj(RegisterId::eax), UnlinkedObj(RegisterId::ebx)));
			}
			break;
		case InterpreterTokenType::Minus:
			this->assertType(leftType, IdentityType::Double);
			this->program.addCode(OpCode::sub(UnlinkedObj(RegisterId::eax), UnlinkedObj(RegisterId::ebx)));
			this->program.addCode(OpCode::neg(UnlinkedObj(RegisterId::eax)));
			break;
		}
	}

	return leftType;
}

IdentityType Parser::doCompare() {
	IdentityType leftType = this->doAdd();

	while (this->isNotEnd() && this->isCompareOp()) {
		this->program.addCode(OpCode::push(UnlinkedObj(RegisterId::eax)));
		InterpreterTokenType comareOp = this->token->type;
		this->match(comareOp);
		this->assertType(this->doAdd(), leftType);

		this->program.addCode(OpCode::pop(UnlinkedObj(RegisterId::ebx)));
		this->program.addCode(OpCode::cmp(UnlinkedObj(RegisterId::ebx), UnlinkedObj(RegisterId::eax)));

		switch (comareOp) {
			case InterpreterTokenType::Equals:
				this->program.addCode(OpCode::se(UnlinkedObj(RegisterId::eax)));
				break;
			case InterpreterTokenType::NotEquals:
				this->program.addCode(OpCode::sne(UnlinkedObj(RegisterId::eax)));
				break;
			case InterpreterTokenType::Greater:
				this->assertType(leftType, IdentityType::Double);
				this->program.addCode(OpCode::sa(UnlinkedObj(RegisterId::eax)));
				break;
			case InterpreterTokenType::GreaterEquals:
				this->assertType(leftType, IdentityType::Double);
				this->program.addCode(OpCode::sae(UnlinkedObj(RegisterId::eax)));
				break;
			case InterpreterTokenType::Lesser:
				this->assertType(leftType, IdentityType::Double);
				this->program.addCode(OpCode::sb(UnlinkedObj(RegisterId::eax)));
				break;
			case InterpreterTokenType::LesserEquals:
				this->assertType(leftType, IdentityType::Double);
				this->program.addCode(OpCode::sbe(UnlinkedObj(RegisterId::eax)));
				break;
		}
		leftType = IdentityType::Bool;
	}

	return leftType;
}

IdentityType Parser::doAnd() {
	IdentityType leftType = this->doCompare();

	while (this->isNotEnd() && this->isAndOp()) {
		this->assertType(leftType, IdentityType::Bool);
		this->match(InterpreterTokenType::And);

		size_t shortCircuitBranch = this->newBranch();
		this->program.addCode(OpCode::test(UnlinkedObj(RegisterId::eax)));
		this->program.addCode(OpCode::je(shortCircuitBranch));

		this->assertType(this->doCompare(), IdentityType::Bool);

		this->program.addCode(OpCode::label(shortCircuitBranch));
	}

	return leftType;
}

IdentityType Parser::doOr() {
	IdentityType leftType = this->doAnd();

	while (this->isNotEnd() && this->isOrOp()) {
		this->assertType(leftType, IdentityType::Bool);
		this->match(InterpreterTokenType::Or);

		size_t shortCircuitBranch = this->newBranch();
		this->program.addCode(OpCode::test(UnlinkedObj(RegisterId::eax)));
		this->program.addCode(OpCode::jne(shortCircuitBranch));

		this->assertType(this->doAnd(), IdentityType::Bool);

		this->program.addCode(OpCode::label(shortCircuitBranch));
	}

	return leftType;
}

IdentityType Parser::doExpression() {
	IdentityType returnType = this->doOr();

	while (this->isNotEnd() && this->isTernaryOp()) {
		this->assertType(returnType, IdentityType::Bool);

		this->match(InterpreterTokenType::Question);

		size_t falseBranch = this->newBranch();
		size_t doneBranch = this->newBranch();

		this->program.addCode(OpCode::test(UnlinkedObj(RegisterId::eax)));
		this->program.addCode(OpCode::je(falseBranch));

		returnType = this->doExpression();

		this->match(InterpreterTokenType::Colon);

		this->program.addCode(OpCode::jmp(doneBranch));
		this->program.addCode(OpCode::label(falseBranch));

		this->assertType(this->doExpression(), returnType);

		this->program.addCode(OpCode::label(doneBranch));
	}

	return returnType;
}


void Parser::doIf(optional<size_t> breakToBranch, optional<size_t> returnToBranch, optional<IdentityType> returnType) {
	size_t elseLabel = this->newBranch();

	this->match(InterpreterTokenType::If);
	this->match(InterpreterTokenType::LeftParen);

	if (this->doExpression() != IdentityType::Bool) {
		this->program.addCode(OpCode::toBool(UnlinkedObj(RegisterId::eax)));
	}

	this->program.addCode(OpCode::test(UnlinkedObj(RegisterId::eax)));
	this->program.addCode(OpCode::je(elseLabel));

	this->match(InterpreterTokenType::RightParen);

	this->doBlock(breakToBranch, returnToBranch, false, true, false, returnType);

	if (this->isNotEnd() && this->token->type == InterpreterTokenType::Else) {
		size_t endLabel = this->newBranch();
		this->program.addCode(OpCode::jmp(endLabel));
		this->program.addCode(OpCode::label(elseLabel));

		this->match(InterpreterTokenType::Else);

		this->doBlock(breakToBranch, returnToBranch, false, true, false, returnType);
		this->program.addCode(OpCode::label(endLabel));
	} else {
		this->program.addCode(OpCode::label(elseLabel));
	}

	//this->program.addCode(OpCode::codeLine(""));
}

void Parser::doWhile(optional<size_t> returnToBranch, optional<IdentityType> returnType) {
	size_t loopLabel = this->newBranch();
	size_t endLabel = this->newBranch();

	this->match(InterpreterTokenType::While);
	this->match(InterpreterTokenType::LeftParen);

	this->program.addCode(OpCode::label(loopLabel));
	if (this->doExpression() != IdentityType::Bool) {
		this->program.addCode(OpCode::toBool(UnlinkedObj(RegisterId::eax)));
	}
	this->program.addCode(OpCode::test(UnlinkedObj(RegisterId::eax)));
	this->program.addCode(OpCode::je(endLabel));
	this->match(InterpreterTokenType::RightParen);

	this->doBlock(endLabel, returnToBranch, false, true, false, returnType);

	this->program.addCode(OpCode::jmp(loopLabel));
	this->program.addCode(OpCode::label(endLabel));
}

void Parser::doLoop(optional<size_t> returnToBranch, optional<IdentityType> returnType) {
	size_t loopLabel = this->newBranch();
	size_t endLabel = this->newBranch();

	this->match(InterpreterTokenType::Loop);

	this->program.addCode(OpCode::label(loopLabel));

	this->doBlock(endLabel, returnToBranch, false, true, false, returnType);

	this->match(InterpreterTokenType::While);
	this->match(InterpreterTokenType::LeftParen);
	if (this->doExpression() != IdentityType::Bool) {
		this->program.addCode(OpCode::toBool(UnlinkedObj(RegisterId::eax)));
	}
	this->match(InterpreterTokenType::RightParen);

	this->program.addCode(OpCode::test(UnlinkedObj(RegisterId::eax)));
	this->program.addCode(OpCode::jne(loopLabel));
	this->program.addCode(OpCode::label(endLabel));
}

void Parser::doFor(optional<size_t> returnToBranch, optional<IdentityType> returnType) {
	size_t compareLabel = this->newBranch();
	size_t incLabel = this->newBranch();
	size_t blockLabel = this->newBranch();
	size_t endLabel = this->newBranch();

	this->pushScope();

	this->match(InterpreterTokenType::For);
	this->match(InterpreterTokenType::LeftParen);

	if (this->isNotEnd() && this->token->type != InterpreterTokenType::LineDelim) {
		this->doAssignOrDeclare(true);
	} else {
		this->match(InterpreterTokenType::LineDelim);
	}

	this->program.addCode(OpCode::label(compareLabel));

	if (this->isNotEnd() && this->token->type != InterpreterTokenType::LineDelim) {
		if (this->doExpression() != IdentityType::Bool) {
			this->program.addCode(OpCode::toBool(UnlinkedObj(RegisterId::eax)));
		}

		this->program.addCode(OpCode::test(UnlinkedObj(RegisterId::eax)));
		this->program.addCode(OpCode::je(endLabel));
	}
	this->program.addCode(OpCode::jmp(blockLabel));
	this->program.addCode(OpCode::label(incLabel));
	this->match(InterpreterTokenType::LineDelim);

	if (this->isNotEnd() && this->token->type != InterpreterTokenType::RightParen) {
		this->doAssignment(false);
	}
	this->match(InterpreterTokenType::RightParen);
	this->program.addCode(OpCode::jmp(compareLabel));
	this->program.addCode(OpCode::label(blockLabel));

	this->doBlock(endLabel, returnToBranch, false, true, false, returnType);

	this->program.addCode(OpCode::jmp(incLabel));
	this->program.addCode(OpCode::label(endLabel));

	this->popScope();
	//this->program.addCode(OpCode::codeLine(""));
}

void Parser::doBreak(optional<size_t> breakToBranch) {
	this->match(InterpreterTokenType::Break);
	if (breakToBranch == nullopt) this->throwError("no loop to break from");

	this->program.addCode(OpCode::jmp(*breakToBranch));

	this->match(InterpreterTokenType::LineDelim);
}

void Parser::doExit() {
	this->match(InterpreterTokenType::Exit);
	if (this->isNotEnd() && this->token->type != InterpreterTokenType::LineDelim) {
		IdentityType exitWithType = this->doExpression();
		if (this->exitType != nullopt) {
			if (exitWithType != *this->exitType) {
				this->matchType(exitWithType, *this->exitType);
			}
		}
		this->program.addCode(OpCode::exit(UnlinkedObj(RegisterId::eax)));
	} else {
		this->program.addCode(OpCode::exit(UnlinkedObj()));
	}
	this->match(InterpreterTokenType::LineDelim);
}

void Parser::doAssignOrDeclare(bool cantBeFunction = false) {
	switch (this->token->type) {
	case InterpreterTokenType::Bool:
	case InterpreterTokenType::Double:
	case InterpreterTokenType::String:
		this->doDeclare(cantBeFunction);
		break;
	default:
		this->doAssignment(true);
		break;
	}
}

void Parser::doDeclare(bool cantBeFunction = false) {
	IdentityType declareType = IdentityType::Null;

	switch (this->token->type) {
		case InterpreterTokenType::Double:
			declareType = IdentityType::Double;
			break;
		case InterpreterTokenType::Bool:
			declareType = IdentityType::Bool;
			break;
		case InterpreterTokenType::String:
			declareType = IdentityType::String;
			break;
		default:
			this->throwError("unknown data type in declaration");
	}
	this->match(this->token->type);

	bool isFirstOne = true;
	do {
		string varName = this->token->sValue;
		this->match(InterpreterTokenType::Ident);

		if (this->isNotEnd() && !cantBeFunction && isFirstOne && this->token->type == InterpreterTokenType::LeftParen) {
			this->doFunction(varName, declareType);
			return;
		} else {
			ScopeObj identObj = this->addVar(varName, declareType);
			UnlinkedObj unlinkedVar = UnlinkedObj(declareType, identObj.scope, identObj.index);
			switch (declareType) {
				case IdentityType::Double:
					this->program.addCode(OpCode::allocDouble(unlinkedVar));
					break;
				case IdentityType::Bool:
					this->program.addCode(OpCode::allocBool(unlinkedVar));
					break;
				case IdentityType::String:
					this->program.addCode(OpCode::allocString(unlinkedVar));
					break;
				default:
					this->throwError("unknown data type in declaration");
			}
			if (this->isNotEnd() && this->token->type == InterpreterTokenType::Assignment) {
				this->match(InterpreterTokenType::Assignment);
				this->assertType(this->doExpression(), declareType);
				this->program.addCode(OpCode::mov(unlinkedVar, UnlinkedObj(RegisterId::eax)));
			}
		}
		if (this->isNotEnd() && this->token->type == InterpreterTokenType::Comma) this->match(InterpreterTokenType::Comma);
		isFirstOne = false;
	} while (this->isNotEnd() && this->token->type != InterpreterTokenType::LineDelim);

	this->match(InterpreterTokenType::LineDelim);
}

void Parser::doFunction(string name, IdentityType type) {
	size_t returnToBranch = this->newBranch();
	size_t skipFuncBranch = this->newBranch();
	size_t funcBlockBranch = this->newBranch();

	this->program.addCode(OpCode::jmp(skipFuncBranch));

	this->pushAllocScope();
	size_t funcAddress = this->program.addCode(OpCode::label(funcBlockBranch));

	vector<IdentityType> paramTypes;
	vector<string> paramIdents;
	vector<ScopeObj> paramObjs;

	this->match(InterpreterTokenType::LeftParen);

	while (this->isNotEnd() && this->token->type != InterpreterTokenType::RightParen) {
		switch (this->token->type) {
			case InterpreterTokenType::Double:
				paramTypes.push_back(IdentityType::Double);
				break;
			case InterpreterTokenType::Bool:
				paramTypes.push_back(IdentityType::Bool);
				break;
			case InterpreterTokenType::String:
				paramTypes.push_back(IdentityType::String);
				break;
			default:
				this->throwError("unexpected token in parameter list " + TokenTypeToString(this->token->type));
		}
		this->match(this->token->type);

		if (this->isNotEnd()) paramIdents.push_back(this->token->sValue);
		this->match(InterpreterTokenType::Ident);

		if (this->isNotEnd() && this->token->type == InterpreterTokenType::Comma) {
			this->match(InterpreterTokenType::Comma);
			if (this->isNotEnd() && this->token->type == InterpreterTokenType::RightParen) this->throwError("expected another parameter, but got a )");
		}
	}
	this->match(InterpreterTokenType::RightParen);

	ScopeObj identObj = this->addFunction(name, type, funcBlockBranch, paramTypes);

	this->pushScope();
	for (size_t i = 0; i < paramIdents.size(); i++) {
		ScopeObj obj = this->addVar(paramIdents[i], paramTypes[i]);
		paramObjs.push_back(obj);
	}

	this->doBlock(nullopt, returnToBranch, true, true, true, type);

	this->popScope();
	this->program.addCode(OpCode::mov(UnlinkedObj(RegisterId::eax), UnlinkedObj()));
	this->program.addCode(OpCode::label(returnToBranch));
	this->program.addCode(OpCode::popScope(this->allocScopeIndex));
	this->program.addCode(OpCode::ret());

	vector<OpCode> funcPreamble;
	funcPreamble.push_back(OpCode::pushScope(this->allocScopeIndex, this->allocScope[this->allocScopeIndex]));

	for (int i = paramObjs.size() - 1; i >= 0; i--) {
		UnlinkedObj unlinkedParam = UnlinkedObj(paramObjs[i].type, paramObjs[i].scope, paramObjs[i].index);
		switch (paramObjs[i].type) {
			case IdentityType::Bool:
				funcPreamble.push_back(OpCode::allocBool(unlinkedParam));
				break;
			case IdentityType::Double:
				funcPreamble.push_back(OpCode::allocDouble(unlinkedParam));
				break;
			case IdentityType::String:
				funcPreamble.push_back(OpCode::allocString(unlinkedParam));
				break;
			default:
				this->throwError("unexpected token in parameter list " + TokenTypeToString(this->token->type));
		}
		funcPreamble.push_back(OpCode::pop(unlinkedParam));
	}

	this->program.insertCode(funcPreamble, funcAddress);
	
	this->popAllocScope();
	this->program.addCode(OpCode::label(skipFuncBranch));
}


void Parser::doReturn(optional<size_t> returnToBranch, optional<IdentityType> returnType) {
	if (returnToBranch == nullopt) this->throwError("tried to return outside of function scope");

	this->match(InterpreterTokenType::Return);

	if (this->isNotEnd() && this->token->type != InterpreterTokenType::LineDelim) {
		if (returnType == nullopt) this->throwError("tried to return expression from typeless function");
		this->assertType(this->doExpression(), *returnType);
	} else {
		this->program.addCode(OpCode::mov(UnlinkedObj(RegisterId::eax), UnlinkedObj()));
	}
	this->program.addCode(OpCode::jmp(*returnToBranch));
	this->match(InterpreterTokenType::LineDelim);
}

void Parser::doAssignment(bool wantsDelim = true) {
	string varName = this->token->sValue;

	this->match(InterpreterTokenType::Ident);
	ScopeObj* identObj = this->getIdentity(varName);
	if (!identObj) this->throwError("tried to do assignment to undefined '" + varName + "'");
	this->match(InterpreterTokenType::Assignment);
	this->assertType(this->doExpression(), identObj->type);

	this->program.addCode(OpCode::mov(UnlinkedObj(identObj->type, identObj->scope, identObj->index), UnlinkedObj(RegisterId::eax)));
	if (wantsDelim) this->match(InterpreterTokenType::LineDelim);
}

void Parser::doIdentStatement() {
	string identName = this->token->sValue;

	ScopeObj* identObj = this->getIdentity(identName);
	if (!identObj) this->throwError("trying to operate on undefined '" + identName + "'");

	switch (identObj->type) {
	case IdentityType::Double:
	case IdentityType::Bool:
	case IdentityType::String:
		this->doAssignment();
		break;
	case IdentityType::Function:
		this->doFuncCall();
		this->match(InterpreterTokenType::LineDelim);
		break;
	default:
		this->throwError("Invalid identity type "+identName+":"+IdentityTypeToString(identObj->type));
	}
}

void Parser::doStatement(optional<size_t> breakToBranch, optional<size_t> returnToBranch, optional<IdentityType> returnType) {
	switch (this->token->type) {
		case InterpreterTokenType::If:
			this->doIf(breakToBranch, returnToBranch, returnType);
			break;
		case InterpreterTokenType::While:
			this->doWhile(returnToBranch, returnType);
			break;
		case InterpreterTokenType::For:
			this->doFor(returnToBranch, returnType);
			break;
		case InterpreterTokenType::Loop:
			this->doLoop(returnToBranch, returnType);
			break;
		case InterpreterTokenType::Break:
			this->doBreak(breakToBranch);
			break;
		case InterpreterTokenType::Exit:
			this->doExit();
			break;
		case InterpreterTokenType::Return:
			this->doReturn(returnToBranch, returnType);
			break;
		case InterpreterTokenType::Double:
		case InterpreterTokenType::String:
		case InterpreterTokenType::Bool:
			this->doDeclare();
			break;
		case InterpreterTokenType::Ident:
			this->doIdentStatement();
			break;
		case InterpreterTokenType::LeftCurly:
			this->doBlock(breakToBranch, returnToBranch, true, false, false, returnType);
			break;
		case InterpreterTokenType::LineDelim:
			this->match(InterpreterTokenType::LineDelim);
			break;
		case InterpreterTokenType::EndOfFile:
			this->match(InterpreterTokenType::EndOfFile);
			break;
		
		default:
			this->throwError("unexpected token found " + TokenTypeToString(this->token->type));
	}
}

void Parser::doBlock(optional<size_t> breakToBranch, optional<size_t> returnToBranch, bool ifNeedsCurlys, bool couldBeStatement, bool dontPushScope = false, optional<IdentityType> returnType=nullopt) {
	if (!dontPushScope) this->pushScope();

	bool hasCurlys = false;

	if (!ifNeedsCurlys && !this->isNotEnd()) return;

	if (ifNeedsCurlys || (this->isNotEnd() && this->token->type==InterpreterTokenType::LeftCurly)) {
		this->match(InterpreterTokenType::LeftCurly);
		hasCurlys = true;
	}

	while (this->isNotEnd()) {
		if (this->token->type == InterpreterTokenType::RightCurly) {
			if (hasCurlys) {
				this->match(InterpreterTokenType::RightCurly);
				hasCurlys = false;
				break;
			} else {
				this->throwError("unexpected token found " + TokenTypeToString(this->token->type));
			}
		}

		this->doStatement(breakToBranch, returnToBranch, returnType);

		if (couldBeStatement && !hasCurlys) break;
	}

	if (hasCurlys) this->throwError("got to end of file without getting closing brace");

	if (!dontPushScope) this->popScope();
}
