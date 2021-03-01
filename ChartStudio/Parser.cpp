#include "stdafx.h"

#include "Parser.h"


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

	this->allocScope.clear();
	this->allocScope.push_back(0);
	this->allocScopeIndex = 0;

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
	return !(a==b || (a==IdentityType::Null || b==IdentityType::Null && strict==false));
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
		this->token = &this->tokens[this->tokenIndex];

		while (this->token && this->token->type == InterpreterTokenType::NewLine) {
			this->program.addCode(OpCode::codeLine(this->token->sValue));
			this->tokenIndex++;
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

void Parser::parse(vector<ExternalDef> externals) {
	this->tokenIndex = this->tokens.size();
	this->firstToken();

	this->branchCounter = 1;
	
	this->scopeIndex = 0;
	this->scopes.clear();

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
	this->program.addCode(OpCode::codeLine(""));

	this->doBlock(NULL, NULL, false, false, true);
	this->program.addCode(OpCode::codeLine(""));
	this->program.addCode( OpCode::exit(UnlinkedObj()) );

	vector<OpCode> mainPreamble = {OpCode::scopeDepth(this->maxScopeDepth)};
	if (this->allocScope[this->allocScopeIndex]) {
		mainPreamble.push_back(OpCode::pushScope(this->allocScopeIndex, this->allocScope[this->allocScopeIndex]));
	}
	this->program.insertCode(mainPreamble, entryPointAddress + 1);

	this->popAllocScope();
}

void Parser::pushAllocScope() {
	this->allocScope.push_back(0);
	this->maxScopeDepth = max(this->maxScopeDepth, this->allocScopeIndex);
}

void Parser::popAllocScope() {
	this->allocScope.pop_back();
}

void Parser::pushScope() {
	this->scopes.push_back({});
}

void Parser::popScope() {
	this->scopes.pop_back();
}


ScopeObj Parser::addToCurrentScope(string name, IdentityType type, size_t branch = 0, const vector<IdentityType> params = {}, IdentityType returnType = IdentityType::Null) {
	bool alreadyExists = this->getIdentity(name, true) != nullptr;
	if (alreadyExists) this->throwError("duplicate name definition, " + name + " already exists in current scope");

	ScopeObj obj = ScopeObj(name, type, branch, params, returnType, this->allocScopeIndex, this->allocScope[this->allocScopeIndex]);

	this->scopes[this->scopeIndex].push_back(obj);

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
	for (size_t i = this->scopeIndex;i>=0;i--){
		for (size_t j = 0; j < this->scopes[i].size(); j++) {
			if (this->scopes[i][j].name == name) return &this->scopes[i][j];
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
	return this->token->type == InterpreterTokenType::And;
}

bool Parser::isAndOp() {
	return this->token->type == InterpreterTokenType::Or;
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
		return this->doFuncCall();

	case IdentityType::String:
	case IdentityType::Double:
	case IdentityType::Bool:
		this->program.addCode(OpCode::mov(UnlinkedObj(RegisterId::eax), UnlinkedObj(identObj->type, identObj->scope, identObj->index)));
		return identObj->type;
	}

	this->throwError("unknown ident type for '" + varName+"'");
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
			this->program.addCode(OpCode::sne( UnlinkedObj(RegisterId::eax) ));
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

	while (this->isNotEnd() && this->isTermOp()) {
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

	this->program.addCode(OpCode::codeLine(""));
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
	this->program.addCode(OpCode::codeLine(""));
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

// 	doFor(returnToBranch, returnType){
// 		const compareLabel = this.newBranch();
// 		const incLabel = this.newBranch();
// 		const blockLabel = this.newBranch();
// 		const endLabel = this.newBranch();


// 		this.pushScope();

// 		this.match(TokenType.For);//								for
// 		this.match(TokenType.LeftParen);//							(

// 		if (this.token?.type!==TokenType.LineDelim){//				[allocate || init]
// 			this.doAssignOrDeclare(true);
// 		}else{
// 			this.match(TokenType.LineDelim);//						;
// 		}

// 		this.program.addLabel( compareLabel );

// 		if (this.token?.type!==TokenType.LineDelim){//				[expression]
// 			let type=this.doExpression();
// 			if (type!==IdentityType.Bool){
// 				this.program.addToBool( Program.unlinkedReg("eax") );
// 			}
// 			this.program.addTest( Program.unlinkedReg("eax") );
// 			this.program.addJE( endLabel );
// 		}

// 		this.program.addJmp( blockLabel );
// 		this.program.addLabel( incLabel );

// 		this.match(TokenType.LineDelim);//							;

// 		if (this.token?.type!==TokenType.RightParen){//				[assignment] 
// 			this.doAssignment(false);
// 		}

// 		this.program.addJmp( compareLabel );

// 		this.match(TokenType.RightParen);//							)

// 		this.program.addLabel( blockLabel );

// 		this.doBlock(endLabel, returnToBranch, false, true, false, returnType);//{ block }

// 		this.program.addJmp( incLabel );
// 		this.program.addLabel( endLabel );

// 		this.popScope();
// 		this.program.addCodeLine(null);
// 	}

// 	doBreak(breakToBranch){
// 		this.match(TokenType.Break);
// 		if (breakToBranch===null || breakToBranch===undefined) this.throwError("no loop to break from");
// 		this.program.addJmp( breakToBranch );

// 		this.match(TokenType.LineDelim);
// 	}

// 	doExit(){
// 		this.match(TokenType.Exit);
// 		if (this.token?.type !== TokenType.LineDelim){
// 			this.doExpression();
// 			this.program.addExit( Program.unlinkedReg("eax") );
// 		}else{
// 			this.program.addExit( Program.unlinkedNull() );
// 		}
// 		this.match(TokenType.LineDelim);
// 	}

// 	doAssignOrDeclare(cantBeFunction=false){
// 		switch (this.token.type){
// 			case TokenType.Bool:
// 			case TokenType.Double:
// 			case TokenType.String:
// 				 this.doDeclare(cantBeFunction);
// 				 break;
// 			default:
// 				this.doAssignment(true); 
// 				break;
// 		}
// 	}

// 	doDeclare(cantBeFunction=false){
// 		let declareType=null;
// 		switch (this.token?.type){
// 			case TokenType.Double:
// 				declareType=IdentityType.Double;
// 				break;
// 			case TokenType.Bool:
// 				declareType=IdentityType.Bool;
// 				break;
// 			case TokenType.String:
// 				declareType=IdentityType.String;
// 				break;
// 			default:
// 				this.throwError("unknown data type in declaration");
// 		}
// 		this.match(this.token.type);

// 		let isFirstOne=true;

// 		do {
// 			let varName = this.token?.value;
// 			this.match(TokenType.Ident);

// 			if (cantBeFunction===false && isFirstOne && this.token?.type===TokenType.LeftParen){
// 				this.doFunction(varName, declareType);
// 				return;
// 			}else{
// 				let identObj=this.addVar(varName, declareType);
// 				let unlinkedVar=Program.unlinkedVariable(declareType, identObj.scope, identObj.index, varName);
// 				switch (declareType){
// 					case IdentityType.Double:
// 						this.program.addDouble( unlinkedVar );
// 						break;
// 					case IdentityType.Bool:
// 						this.program.addBool( unlinkedVar );
// 						break;
// 					case IdentityType.String:
// 						this.program.addString( unlinkedVar );
// 						break;
// 					default:
// 						this.throwError("unknown data type in declaration");
// 				}
// 				if (this.token?.type===TokenType.Assignment){
// 					this.match(TokenType.Assignment);
// 					let expType=this.doExpression();
// 					if (this.typesDontMatch(expType,declareType)) this.typeMismatch(declareType, expType);
// 					this.program.addMov( unlinkedVar, Program.unlinkedReg("eax") );
// 				}
// 			}
// 			if (this.token?.type===TokenType.Comma) this.match(TokenType.Comma);

// 			isFirstOne=false;
// 		} while (this.isNotEnd() && this.token.type!==TokenType.LineDelim)
// 		this.match(TokenType.LineDelim);
// 	}


// 	doFunction(name, type){
// 		const returnToBranch=this.newBranch();
// 		const skipFuncBranch = this.newBranch();
// 		const funcBlockBranch = this.newBranch();

// 		this.program.addJmp( skipFuncBranch );

// 		this.pushAllocScope();
// 		const funcAddress=this.program.addLabel( funcBlockBranch );

// 		let paramTypes=[];
// 		let paramIdents=[];
// 		let paramObjs=[];

// 		this.match(TokenType.LeftParen);
// 		while (this.isNotEnd() && this.token.type!==TokenType.RightParen){
// 			switch (this.token.type){
// 				case TokenType.Double:
// 					paramTypes.push(IdentityType.Double);
// 					break;
// 				case TokenType.Bool:
// 					paramTypes.push(IdentityType.Bool);
// 					break;
// 				case TokenType.String:
// 					paramTypes.push(IdentityType.String);
// 					break;
// 				default:
// 					this.throwError("unexpected token in parameter list "+this.token.type.toString());
// 			}
// 			this.match(this.token.type);

// 			paramIdents.push(this.token?.value);
// 			this.match(TokenType.Ident);

// 			if (this.token?.type === TokenType.Comma){
// 				this.match(TokenType.Comma);
// 				if (this.token?.type===TokenType.RightParen) this.throwError("expected another parameter, but got a )");
// 			}
// 		}
// 		this.match(TokenType.RightParen);

// 		let identObj = this.addFunction(name, type, funcBlockBranch, paramTypes);
// 		if (!identObj) this.throwError("failed to add function '"+name+"' to ident list");

// 		this.pushScope();
// 		for (let i=0;i<paramIdents.length;i++){
// 			let obj=this.addVar(paramIdents[i], paramTypes[i]);
// 			if (!obj) this.throwError("attempted to push null param to list on function '"+name+"'")
// 			paramObjs.push(obj);
// 		}

// 		this.doBlock(null, returnToBranch, true, true, true, type);

// 		this.popScope();
// 		this.program.addMov( Program.unlinkedReg("eax"), Program.unlinkedNull() );

// 		this.program.addLabel( returnToBranch );
// 		this.program.addPopScope( this.allocScopeIndex );
// 		this.program.addRet();

// 		let funcPreamble=[];
// 		funcPreamble.push(this.program.addPushScope( this.allocScopeIndex, this.allocScope[this.allocScopeIndex], true ));

// 		for (let i=paramObjs.length-1;i>=0;i--){
// 			let unlinkedParam=Program.unlinkedVariable(paramObjs[i].type, paramObjs[i].scope, paramObjs[i].index, paramObjs[i].name);
// 			switch (paramObjs[i].type){
// 			case IdentityType.Bool:
// 				funcPreamble.push(this.program.addBool( unlinkedParam, true ));
// 				break;
// 			case IdentityType.Double:
// 				funcPreamble.push(this.program.addDouble( unlinkedParam, true ));
// 				break;
// 			case IdentityType.String:
// 				funcPreamble.push(this.program.addString( unlinkedParam, true ));
// 				break;
// 			default:
// 				this.throwError("unexpected type in parameter list allocation "+paramTypes[i].toString());
// 			}
// 			funcPreamble.push(this.program.addPop( unlinkedParam, true ));
// 		}

// 		this.program.code.splice(funcAddress+1,0,...funcPreamble);


// 		this.popAllocScope();
// 		this.program.addLabel( skipFuncBranch );
// 		this.program.addCodeLine(null);
// 	}

// 	doReturn(returnToBranch, returnType){
// 		this.match(TokenType.Return);

// 		if (this.token?.type!==TokenType.LineDelim){
// 			let expressionType=this.doExpression();
// 			if (this.typesDontMatch(expressionType, returnType)) this.typeMismatch(returnType, expressionType);
// 		}else{
// 			this.program.addMov( Program.unlinkedReg("eax"), Program.unlinkedNull() );
// 		}

// 		this.program.addJmp( returnToBranch );
// 		this.match(TokenType.LineDelim);
// 	}

// 	doAssignment(wantsDelim=true){
// 		let varName=this.token.value;

// 		this.match(TokenType.Ident);
// 		let identObj = this.getIdentity(varName);
// 		if (!identObj) this.throwError("tried to assign to undefined '"+varName+"'");

// 		this.match(TokenType.Assignment);

// 		let expressionType=this.doExpression();
// 		if (this.typesDontMatch(expressionType, identObj.type)) this.typeMismatch(identObj.type, expressionType);

// 		this.program.addMov( Program.unlinkedVariable(identObj.type, identObj.scope, identObj.index, varName), Program.unlinkedReg("eax") );

// 		if (wantsDelim===true) this.match(TokenType.LineDelim);
// 	}

// 	doIdentStatement(){
// 		let identName = this.token.value;

// 		let identObj = this.getIdentity(identName);
// 		if (!identObj) this.throwError("trying to operate on undefined '"+identName+"'");

// 		switch (identObj.type){
// 			case IdentityType.Double:
// 			case IdentityType.Bool:
// 			case IdentityType.String:
// 				this.doAssignment();
// 				break;
// 			case IdentityType.Function:
// 				this.doFuncCall();
// 				this.match(TokenType.LineDelim);
// 				break;
// 			default:
// 				this.throwError("Invalid identity type "+identName+":"+this.symbolToString(identObj.type));
// 		}
// 	}

// 	doStatement(breakToBranch, returnToBranch, returnType){
// 		switch (this.token.type){
// 			case TokenType.If:
// 				this.doIf(breakToBranch, returnToBranch, returnType);
// 				break;
// 			case TokenType.While:
// 				this.doWhile(returnToBranch, returnType);
// 				break;
// 			case TokenType.For:
// 				this.doFor(returnToBranch, returnType);
// 				break;
// 			case TokenType.Loop:
// 				this.doLoop(returnToBranch, returnType);
// 				break;
// 			case TokenType.Break:
// 				this.doBreak(breakToBranch);
// 				break;

// 			case TokenType.Exit:
// 				this.doExit();
// 				break;

// 			case TokenType.Return:
// 				if (returnToBranch!=null && returnToBranch!=undefined){
// 					this.doReturn(returnToBranch, returnType);
// 				}else{
// 					this.throwError("not allowed to return outside of a function");
// 				}
// 				break;

// 			case TokenType.Double:
// 			case TokenType.String:
// 			case TokenType.Bool:
// 				this.doDeclare();
// 				break;

// 			case TokenType.Ident:
// 				this.doIdentStatement();
// 				break;

// 			case TokenType.LeftCurly:
// 				this.doBlock(breakToBranch, returnToBranch, true, false, false, returnType);
// 				break;

// 			case TokenType.LineDelim:
// 				this.match(TokenType.LineDelim);
// 				break;

// 			default:
// 				this.throwError("Unexpected token in block, "+this.symbolToString(this.token.type));
// 		}
// 	}

// 	doBlock(breakToBranch, returnToBranch, ifNeedsCurlys, couldBeStatment, dontPushScope=false, returnType=null){
// 		if (!dontPushScope) this.pushScope();


// 		let hasCurlys = false;

// 		if (!ifNeedsCurlys && !this.isNotEnd()) return;//End of the program, and we're not expecting a closing '}'

// 		if (ifNeedsCurlys || this.token?.type===TokenType.LeftCurly){
// 			this.match(TokenType.LeftCurly);
// 			hasCurlys=true;
// 		}  

// 		while (this.isNotEnd()){
// 			if (this.token.type===TokenType.RightCurly){
// 				if (hasCurlys){
// 					this.match(TokenType.RightCurly);
// 					hasCurlys=false;
// 					break;
// 				} 
// 				this.throwError("Unexpected token in block, "+this.symbolToString(this.token.type));
// 			}
// 			this.doStatement(breakToBranch, returnToBranch, returnType);

// 			if (couldBeStatment && hasCurlys===false) break;
// 		}

// 		if (hasCurlys) this.throwError("Got to the end of the file without getting an expected "+this.symbolToString(TokenType.RightCurly));

// 		if (!dontPushScope) this.popScope();
// 	}
// }

// module.exports={Parser, IdentityType};