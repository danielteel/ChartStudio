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
			//this->program.addCodeLine(this->token->sValue);
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

	//this->program = new Program();

	for (size_t i = 0; i < externals.size(); i++) {
		this->addToCurrentScope(externals[i].name, externals[i].type, i, externals[i].params, externals[i].returnType);
	}

	this->pushAllocScope();

	const size_t entryPoint = this->newBranch();
	//const size_t entryPointAddress = this->program->addLabel(entryPoint);
	//this->program->addCodeLine(nullptr);

	this->doBlock(NULL, NULL, false, false, true);
	//this->program->addCodeLine(nullptr);
	//this->program->addExit( Program::unlinkedNull() );

	vector<OpCode> mainPreamble;

	//mainPreamble.push_back(this->program->addScopeDepth(this->maxScopeDepth, true));
	if (this->allocScope[this->allocScopeIndex]) {
		mainPreamble.push_back(this->program->addPushScope(this->allocScopeIndex, this->allocScope[this->allocScopeIndex], true));
	}
	this->program->code->splice(entryPointAddress+1)
}



// 		this.program.addExit( Program.unlinkedNull() );

// 		let mainPreamble=[];
// 		mainPreamble.push(this.program.addScopeDepth(this.maxScopeDepth, true));
// 		if (this.allocScope[this.allocScopeIndex]){
// 			mainPreamble.push(this.program.addPushScope(this.allocScopeIndex, this.allocScope[this.allocScopeIndex], true));
// 		}
// 		this.program.code.splice(mainPreamble+1, 0, ...mainPreamble);

// 		this.popAllocScope();
// 		return this.program;
// 	}

// 	pushAllocScope(){
// 		this.allocScope[++this.allocScopeIndex]=0;
// 		this.maxScopeDepth=Math.max(this.maxScopeDepth, this.allocScopeIndex);
// 	}
// 	popAllocScope(){
// 		this.allocScope[this.allocScopeIndex--]=undefined;
// 	}

// 	pushScope(){
// 		this.scopes[++this.scopeIndex]=[];
// 	}
// 	popScope(){
// 		this.scopes[this.scopeIndex--]=undefined;
// 	}
// 	addToCurrentScope(name, type, branch=null, params=null, returnType=null){
// 		const alreadyExists=this.getIdentity(name, true);
// 		if (alreadyExists !== null) this.throwError("Duplicate define, "+name+" already exists in current scope as "+alreadyExists.name+":"+alreadyExists.type.toString());
// 		let obj={name: name, type: type, branch: branch, params: params, returnType: returnType, scope: this.allocScopeIndex, index: this.allocScope[this.allocScopeIndex]};
// 		this.scopes[this.scopeIndex].push(obj);
// 		switch (type){
// 			case IdentityType.Bool:
// 			case IdentityType.Double:
// 			case IdentityType.String:
// 				this.allocScope[this.allocScopeIndex]++;
// 				break;
// 		}
// 		return obj;
// 	}

// 	getFromStringPool(string){
// 		let stringIndex=this.stringPool.indexOf(string);
// 		if (stringIndex<0){
// 			this.stringPool.push(string);
// 			stringIndex=this.stringPool.length-1;
// 		}
// 		return stringIndex;
// 	}



// 	getIdentity(name, onlyInCurrentScope=false){
// 		for (let i = this.scopeIndex;i>=0;i--){
// 			let identity = this.scopes[i].find( current => name === current.name );
// 			if (identity) return identity;
// 			if (onlyInCurrentScope) break;
// 		}
// 		return null;
// 	}

// 	addVar(name, type){
// 		return this.addToCurrentScope(name, type);
// 	}

// 	addFunction(name, returnType, branch, params){
// 		return this.addToCurrentScope(name, IdentityType.Function, branch, params, returnType);
// 	}


// 	isPowerOp(){
// 		if (this.token.type===TokenType.Exponent) return true;
// 		return false;
// 	}

// 	isTermOp(){
// 		switch (this.token.type){
// 		case TokenType.Multiply:
// 		case TokenType.Divide:
// 		case TokenType.Mod:
// 			return true;
// 		}
// 		return false;
// 	}

// 	isAddOp(){
// 		if (this.token.type===TokenType.Plus || this.token.type===TokenType.Minus) return true;
// 		return false;
// 	}

// 	isCompareOp(){
// 		switch (this.token.type){
// 			case TokenType.Lesser:
// 			case TokenType.LesserEquals:
// 			case TokenType.Greater:
// 			case TokenType.GreaterEquals:
// 			case TokenType.Equals:
// 			case TokenType.NotEquals:
// 				return true;
// 		}
// 		return false;
// 	}

// 	isAndOp(){
// 		if (this.token.type===TokenType.And) return true;
// 		return false;
// 	}

// 	isOrOp(){
// 		if (this.token.type===TokenType.Or) return true;
// 		return false;
// 	}

// 	isTernaryOp(){
// 		if (this.token.type===TokenType.Question) return true;
// 		return false;
// 	}

// 	doFuncCall(funcName=null){
// 		let needsIdentMatched=false;
// 		if (funcName===null){
// 			funcName=this.token?.value;
// 			needsIdentMatched=true;
// 		}
// 		let identObj = this.getIdentity(funcName);
// 		if (identObj.type!==IdentityType.Function) this.throwError("tried to call a function named '"+funcName+"' that doesnt exist");
// 		if (!identObj) this.throwError("tried to call undefined function'"+funcName+"'");

// 		if (needsIdentMatched) this.match(TokenType.Ident);

// 		this.match(TokenType.LeftParen);

// 		for (let i=0;i<identObj.params.length;i++){
// 			let type=this.doExpression();
// 			this.program.addPush( Program.unlinkedReg("eax") );

// 			if (this.typesDontMatch(type, identObj.params[i])) this.typeMismatch(identObj.params[i], type);

// 			if (i<identObj.params.length-1){
// 				this.match(TokenType.Comma);
// 			}
// 		}

// 		this.match(TokenType.RightParen);

// 		if (identObj.scope===0){
// 			this.program.addExCall(identObj.branch, "fxn "+identObj.name);
// 		}else{
// 			this.program.addCall(identObj.branch, "fxn "+identObj.name);
// 		}

// 		return identObj.returnType;
// 	}

// 	doIdent(){
// 		const varName=this.token.value;
// 		const identObj = this.getIdentity(varName);
// 		this.match(TokenType.Ident);
// 		if (!identObj) this.throwError("tried to access undefined '"+varName+"'");

// 		switch (identObj.type){
// 			case IdentityType.Function:
// 				return this.doFuncCall(varName);

// 			case IdentityType.String:
// 			case IdentityType.Bool:
// 			case IdentityType.Double:
// 				this.program.addMov( Program.unlinkedReg("eax"), Program.unlinkedVariable(identObj.type, identObj.scope, identObj.index, varName) );
// 				return identObj.type;
// 		}
// 		this.throwError("unknown ident type "+varName+":"+identObj.type.toString());
// 	}

// 	doFactor(){
// 		let type=null;
// 		switch (this.token?.type){
// 			case TokenType.Ident:
// 				return this.doIdent();

// 			case TokenType.Minus:
// 				this.match(TokenType.Minus);
// 				type=this.doFactor();
// 				if (this.typesDontMatch(IdentityType.Double, type)) this.typeMismatch(IdentityType.Double, type);
// 				this.program.addNeg( Program.unlinkedReg("eax") );
// 				return IdentityType.Double;

// 			case TokenType.Not:
// 				this.match(TokenType.Not);
// 				type=this.doFactor();
// 				if (this.typesDontMatch(IdentityType.Bool, type)) this.typeMismatch(IdentityType.Bool, type);
// 				this.program.addNot( Program.unlinkedReg("eax") );
// 				return IdentityType.Bool;

// 			case TokenType.Question:
// 				this.match(TokenType.Question);
// 				type=this.doFactor();
// 				this.program.addCmp( Program.unlinkedReg("eax"), Program.unlinkedNull() );
// 				this.program.addSNE( Program.unlinkedReg("eax") );
// 				return IdentityType.Bool;

// 			case TokenType.LeftParen:
// 				this.match(TokenType.LeftParen);
// 				type=this.doExpression();
// 				this.match(TokenType.RightParen);
// 				return type;

// 			case TokenType.Null:
// 				this.program.addMov( Program.unlinkedReg("eax"), Program.unlinkedNull() );
// 				this.match(TokenType.Null);
// 				return IdentityType.Null;

// 			case TokenType.True:
// 				this.program.addMov( Program.unlinkedReg("eax"), Program.unlinkedLiteral(IdentityType.Bool, true) );
// 				this.match(TokenType.True);
// 				return IdentityType.Bool;

// 			case TokenType.False:
// 				this.program.addMov( Program.unlinkedReg("eax"), Program.unlinkedLiteral(IdentityType.Bool, false) );
// 				this.match(TokenType.False);
// 				return IdentityType.Bool;

// 			case TokenType.DoubleLiteral:
// 				this.program.addMov( Program.unlinkedReg("eax"), Program.unlinkedLiteral(IdentityType.Double, this.token.value) );
// 				this.match(TokenType.DoubleLiteral);
// 				return IdentityType.Double;

// 			case TokenType.StringLiteral:
// 				this.program.addMov( Program.unlinkedReg("eax"), Program.unlinkedLiteral(IdentityType.String, this.token.value) );
// 				this.match(TokenType.StringLiteral);
// 				return IdentityType.String;

// 			case TokenType.Bool:
// 				this.match(TokenType.Bool);
// 				this.match(TokenType.LeftParen);
// 				type = this.doExpression();
// 				if (this.typesDontMatch(IdentityType.Bool, type)){
// 					this.program.addToBool( Program.unlinkedReg("eax") );
// 				}
// 				this.match(TokenType.RightParen);
// 				return IdentityType.Bool;

// 			case TokenType.Double:
// 				this.match(TokenType.Double);
// 				this.match(TokenType.LeftParen);
// 				type = this.doExpression();
// 				if (this.typesDontMatch(IdentityType.Double, type)){
// 					this.program.addToDouble( Program.unlinkedReg("eax") );
// 				}
// 				this.match(TokenType.RightParen);
// 				return IdentityType.Double;

// 			case TokenType.String:
// 				this.match(TokenType.String);
// 				this.match(TokenType.LeftParen);
// 				type = this.doExpression();
// 				if (this.token?.type===TokenType.Comma){
// 					this.match(TokenType.Comma);
// 					this.program.addPush( Program.unlinkedReg("eax") );
// 					type=this.doExpression();
// 					if (this.typesDontMatch(type, IdentityType.Double)) this.typeMismatch(IdentityType.Double, type);
// 					this.program.addPop( Program.unlinkedReg("ebx") );
// 					this.program.addToString( Program.unlinkedReg("ebx"), Program.unlinkedReg("eax") );
// 					this.program.addMov( Program.unlinkedReg("eax"), Program.unlinkedReg("ebx") );
// 				} else {
// 					this.program.addToString( Program.unlinkedReg("eax"), Program.unlinkedNull() );
// 				}
// 				this.match(TokenType.RightParen);
// 				return IdentityType.String;

// 			case TokenType.Ceil:
// 				this.match(TokenType.Ceil);
// 				this.match(TokenType.LeftParen);
// 				type=this.doExpression();
// 				if (this.typesDontMatch(type, IdentityType.Double)) this.typeMismatch(IdentityType.Double, type);
// 				this.program.addCeil( Program.unlinkedReg("eax") );
// 				this.match(TokenType.RightParen);
// 				return IdentityType.Double;

// 			case TokenType.Floor:
// 				this.match(TokenType.Floor);
// 				this.match(TokenType.LeftParen);
// 				type=this.doExpression();
// 				if (this.typesDontMatch(type, IdentityType.Double)) this.typeMismatch(IdentityType.Double, type);
// 				this.program.addFloor( Program.unlinkedReg("eax") );
// 				this.match(TokenType.RightParen);
// 				return IdentityType.Double;

// 			case TokenType.Abs:
// 				this.match(TokenType.Abs);
// 				this.match(TokenType.LeftParen);
// 				type=this.doExpression();
// 				if (this.typesDontMatch(type, IdentityType.Double)) this.typeMismatch(IdentityType.Double, type);
// 				this.program.addAbs( Program.unlinkedReg("eax") );
// 				this.match(TokenType.RightParen);
// 				return IdentityType.Double;

// 			case TokenType.Min:
// 				this.match(TokenType.Min);
// 				this.match(TokenType.LeftParen);
// 				type=this.doExpression();
// 				this.program.addPush( Program.unlinkedReg("eax") );
// 				if (this.typesDontMatch(type, IdentityType.Double)) this.typeMismatch(IdentityType.Double, type);
// 				this.match(TokenType.Comma);
// 				type=this.doExpression();
// 				if (this.typesDontMatch(type, IdentityType.Double)) this.typeMismatch(IdentityType.Double, type);
// 				this.program.addPop( Program.unlinkedReg("ebx") );
// 				this.program.addMin( Program.unlinkedReg("eax"), Program.unlinkedReg("ebx") );
// 				this.match(TokenType.RightParen);
// 				return IdentityType.Double;

// 			case TokenType.Max:
// 				this.match(TokenType.Max);
// 				this.match(TokenType.LeftParen);
// 				type=this.doExpression();
// 				this.program.addPush( Program.unlinkedReg("eax") );
// 				if (this.typesDontMatch(type, IdentityType.Double)) this.typeMismatch(IdentityType.Double, type);
// 				this.match(TokenType.Comma);
// 				type=this.doExpression();
// 				if (this.typesDontMatch(type, IdentityType.Double)) this.typeMismatch(IdentityType.Double, type);
// 				this.program.addPop( Program.unlinkedReg("ebx") );
// 				this.program.addMax( Program.unlinkedReg("eax"), Program.unlinkedReg("ebx") );
// 				this.match(TokenType.RightParen);
// 				return IdentityType.Double;

// 			case TokenType.Clamp:

// 				this.match(TokenType.Clamp);
// 				this.match(TokenType.LeftParen);
// 				this.matchType(this.doExpression(), IdentityType.Double);

// 				this.program.addPush( Program.unlinkedReg("eax") );

// 				this.match(TokenType.Comma);
// 				this.matchType(this.doExpression(), IdentityType.Double);

// 				this.program.addPop( Program.unlinkedReg("ebx") );
// 				this.program.addMax( Program.unlinkedReg("eax"), Program.unlinkedReg("ebx") );
// 				this.program.addPush( Program.unlinkedReg("eax") );

// 				this.match(TokenType.Comma);
// 				this.matchType(this.doExpression(), IdentityType.Double);

// 				this.program.addPop( Program.unlinkedReg("ebx") );
// 				this.program.addMin( Program.unlinkedReg("eax"), Program.unlinkedReg("ebx") );

// 				this.match(TokenType.RightParen);
// 				return IdentityType.Double;

// 			case TokenType.Len:
// 				this.match(TokenType.Len);
// 				this.match(TokenType.LeftParen);
// 				this.matchType(this.doExpression(), IdentityType.String);
// 				this.program.addLen( Program.unlinkedReg("eax") );
// 				this.match(TokenType.RightParen);
// 				return IdentityType.Double;

// 			case TokenType.SubStr:
// 				this.match(TokenType.SubStr);
// 				this.match(TokenType.LeftParen);
// 				this.matchType(this.doExpression(), IdentityType.String);

// 				this.program.addPush( Program.unlinkedReg("eax") );

// 				this.match(TokenType.Comma);
// 				this.matchType(this.doExpression(), IdentityType.Double);

// 				this.program.addPush( Program.unlinkedReg("eax") );

// 				this.match(TokenType.Comma);
// 				this.matchType(this.doExpression(), IdentityType.Double);

// 				this.program.addMov( Program.unlinkedReg("ecx"), Program.unlinkedReg("eax") );
// 				this.program.addPop( Program.unlinkedReg("ebx") );
// 				this.program.addPop( Program.unlinkedReg("eax") );
// 				this.program.addSubStr( Program.unlinkedReg("eax"), Program.unlinkedReg("ebx"), Program.unlinkedReg("ecx") );
// 				this.match(TokenType.RightParen);
// 				return IdentityType.String;

// 			case TokenType.Trim:
// 				this.match(TokenType.Trim);
// 				this.match(TokenType.LeftParen);
// 				this.matchType(this.doExpression(), IdentityType.String);
// 				this.program.addTrim( Program.unlinkedReg("eax") );
// 				this.match(TokenType.RightParen);
// 				return IdentityType.String;

// 			case TokenType.LCase:
// 				this.match(TokenType.LCase);
// 				this.match(TokenType.LeftParen);
// 				this.matchType(this.doExpression(), IdentityType.String);
// 				this.program.addLCase( Program.unlinkedReg("eax") );
// 				this.match(TokenType.RightParen);
// 				return IdentityType.String;

// 			case TokenType.UCase:
// 				this.match(TokenType.UCase);
// 				this.match(TokenType.LeftParen);
// 				this.matchType(this.doExpression(), IdentityType.String);
// 				this.program.addUCase( Program.unlinkedReg("eax") );
// 				this.match(TokenType.RightParen);
// 				return IdentityType.String;

// 			default:
// 				this.throwError("expected factor but found "+this.symbolToString(this.token.type));
// 		}
// 	}

// 	doExponentiation(){
// 		let leftType=this.doFactor();

// 		while (this.isNotEnd() && this.isPowerOp()){
// 			this.program.addPush( Program.unlinkedReg("eax") );

// 			let powerOp=this.token.type;
// 			this.match(powerOp);
// 			let rightType=this.doFactor();
// 			if (this.typesDontMatch(leftType, IdentityType.Double)) this.typeMismatch(IdentityType.Double, leftType);
// 			if (this.typesDontMatch(rightType, IdentityType.Double)) this.typeMismatch(IdentityType.Double, rightType);

// 			this.program.addPop( Program.unlinkedReg("ebx") );

// 			switch (powerOp){
// 				case TokenType.Exponent:
// 					this.program.addExponent( Program.unlinkedReg("ebx"), Program.unlinkedReg("eax") );
// 					this.program.addMov( Program.unlinkedReg("eax"), Program.unlinkedReg("ebx") );
// 					break;
// 			}

// 			leftType=rightType;
// 		}

// 		return leftType;
// 	}

// 	doTerm(){
// 		let leftType=this.doExponentiation();

// 		while (this.isNotEnd() && this.isTermOp()){
// 			this.program.addPush( Program.unlinkedReg("eax") );

// 			let termOp=this.token.type;
// 			this.match(termOp);
// 			let rightType=this.doExponentiation();
// 			if (this.typesDontMatch(leftType, IdentityType.Double)) this.typeMismatch(IdentityType.Double, leftType);
// 			if (this.typesDontMatch(rightType, IdentityType.Double)) this.typeMismatch(IdentityType.Double, rightType);

// 			this.program.addPop( Program.unlinkedReg("ebx") );

// 			switch (termOp){
// 				case TokenType.Multiply:
// 					this.program.addMul( Program.unlinkedReg("eax"), Program.unlinkedReg("ebx") );
// 					break;
// 				case TokenType.Divide:
// 					this.program.addDiv( Program.unlinkedReg("ebx"), Program.unlinkedReg("eax") );
// 					this.program.addMov( Program.unlinkedReg("eax"), Program.unlinkedReg("ebx") );
// 					break;
// 				case TokenType.Mod:
// 					this.program.addMod( Program.unlinkedReg("ebx"), Program.unlinkedReg("eax") );
// 					this.program.addMov( Program.unlinkedReg("eax"), Program.unlinkedReg("ebx") );
// 					break;
// 			}

// 			leftType=rightType;
// 		}

// 		return leftType;
// 	}

// 	doAdd(){
// 		let leftType=this.doTerm();

// 		while (this.isNotEnd() && this.isAddOp()){
// 			this.program.addPush( Program.unlinkedReg("eax") );

// 			let addOp=this.token.type;
// 			this.match(addOp);
// 			let rightType=this.doTerm();

// 			if (this.typesDontMatch(leftType, rightType)) this.typeMismatch(leftType, rightType);

// 			this.program.addPop( Program.unlinkedReg("ebx") );

// 			switch (addOp){
// 				case TokenType.Plus:
// 					if (leftType===IdentityType.String){
// 						this.program.addConcat( Program.unlinkedReg("ebx"), Program.unlinkedReg("eax") );
// 						this.program.addMov( Program.unlinkedReg("eax"), Program.unlinkedReg("ebx") );
// 					}else if (leftType===IdentityType.Double){
// 						this.program.addAdd( Program.unlinkedReg("eax"), Program.unlinkedReg("ebx") );
// 					}else{
// 						this.throwError("'+' operator only valid for strings or doubles");
// 					}
// 					break;
// 				case TokenType.Minus:
// 					if (leftType!==IdentityType.Double) this.throwError("'-' operator only valid for doubles");
// 					this.program.addSub( Program.unlinkedReg("eax"), Program.unlinkedReg("ebx") );
// 					this.program.addNeg( Program.unlinkedReg("eax") );
// 					break;
// 			}

// 			leftType=rightType;
// 		}

// 		return leftType;
// 	}

// 	doCompare(){
// 		let leftType=this.doAdd();

// 		while (this.isNotEnd() && this.isCompareOp()){
// 			this.program.addPush( Program.unlinkedReg("eax") );
// 			let compareOp=this.token.type;
// 			this.match(compareOp);
// 			let rightType=this.doAdd();

// 			if (this.typesDontMatch(leftType, rightType)) this.typeMismatch(leftType, rightType);


// 			this.program.addPop( Program.unlinkedReg("ebx") );
// 			this.program.addCmp( Program.unlinkedReg("ebx"), Program.unlinkedReg("eax") );

// 			switch (compareOp){
// 				case TokenType.Equals:
// 					this.program.addSE( Program.unlinkedReg("eax") );
// 					break;
// 				case TokenType.NotEquals:
// 					this.program.addSNE( Program.unlinkedReg("eax") );
// 					break;
// 				case TokenType.Greater:
// 					if (leftType!==IdentityType.Double) this.throwError("'>' operator only valid for double types");
// 					this.program.addSA( Program.unlinkedReg("eax") );
// 					break;
// 				case TokenType.GreaterEquals:
// 					if (leftType!==IdentityType.Double) this.throwError("'>=' operator only valid for double types");
// 					this.program.addSAE( Program.unlinkedReg("eax") );
// 					break;
// 				case TokenType.Lesser:
// 					if (leftType!==IdentityType.Double) this.throwError("'<' operator only valid for double types");
// 					this.program.addSB( Program.unlinkedReg("eax") );
// 					break;
// 				case TokenType.LesserEquals:
// 					if (leftType!==IdentityType.Double) this.throwError("'<=' operator only valid for double types");
// 					this.program.addSBE( Program.unlinkedReg("eax") );
// 					break;
// 			}

// 			leftType=IdentityType.Bool;
// 		}

// 		return leftType;
// 	}

// 	doAnd(){
// 		let leftType=this.doCompare();

// 		while (this.isNotEnd() && this.isAndOp()){
// 			if (this.typesDontMatch(IdentityType.Bool, leftType)) this.typeMismatch(IdentityType.Bool, leftType);

// 			let shortCircuitBranch=this.newBranch();
// 			this.program.addCmp( Program.unlinkedReg("eax"), Program.unlinkedLiteral(IdentityType.Bool, true) );
// 			this.program.addJNE( shortCircuitBranch );

// 			this.program.addPush( Program.unlinkedReg("eax") );

// 			this.match(TokenType.And);

// 			let rightType=this.doCompare();
// 			if (this.typesDontMatch(IdentityType.Bool, rightType)) this.typeMismatch(IdentityType.Bool, rightType);

// 			this.program.addPop( Program.unlinkedReg("ebx") );
// 			this.program.addAnd( Program.unlinkedReg("eax"), Program.unlinkedReg("ebx") );
// 			this.program.addLabel( shortCircuitBranch );
// 		}

// 		return leftType;
// 	}

// 	doOr(){
// 		let leftType=this.doAnd();

// 		while (this.isNotEnd() && this.isOrOp()){
// 			if (this.typesDontMatch(IdentityType.Bool, leftType)) this.typeMismatch(IdentityType.Bool, leftType);

// 			let shortCircuitBranch=this.newBranch();
// 			this.program.addCmp( Program.unlinkedReg("eax"), Program.unlinkedLiteral(IdentityType.Bool, true) );
// 			this.program.addJE( shortCircuitBranch );

// 			this.program.addPush( Program.unlinkedReg("eax") );

// 			this.match(TokenType.Or);

// 			let rightType=this.doAnd();
// 			if (this.typesDontMatch(IdentityType.Bool, rightType)) this.typeMismatch(IdentityType.Bool, rightType);

// 			this.program.addPop( Program.unlinkedReg("ebx") );
// 			this.program.addOr( Program.unlinkedReg("eax"), Program.unlinkedReg("ebx") );
// 			this.program.addLabel( shortCircuitBranch );
// 		}

// 		return leftType;
// 	}

// 	doExpression(){
// 		let leftType=this.doOr();
// 		let returnType=null;

// 		while (this.isNotEnd() && this.isTernaryOp()){
// 			if (this.typesDontMatch(IdentityType.Bool, leftType)) this.typeMismatch(IdentityType.Bool, leftType);


// 			this.match(TokenType.Question);

// 			let falseBranch=this.newBranch();
// 			let doneBranch=this.newBranch();
// 			this.program.addCmp( Program.unlinkedReg("eax"), Program.unlinkedLiteral(IdentityType.Bool, true) );
// 			this.program.addJNE( falseBranch );

// 			let trueType=this.doExpression();

// 			if (returnType && this.typesDontMatch(returnType, trueType)) this.throwError("expected chained ternary operators to evaluate to same type");
// 			if (!returnType) returnType=trueType;

// 			this.match(TokenType.Colon);

// 			this.program.addJmp( doneBranch );
// 			this.program.addLabel( falseBranch );

// 			let falseType=this.doExpression();

// 			if (this.typesDontMatch(trueType, falseType))  this.throwError("expected ternary true/false branches to evaluate to same type");

// 			this.program.addLabel( doneBranch );
// 		}

// 		if (returnType){
// 			return returnType;
// 		}else{
// 			return leftType;
// 		}
// 	}

// 	doIf(breakToBranch, returnToBranch, returnType){
// 		const elseLabel = this.newBranch();

// 		this.match(TokenType.If);

// 		this.match(TokenType.LeftParen);
// 		let type=this.doExpression();
// 		if (type!==IdentityType.Bool){
// 			this.program.addToBool( Program.unlinkedReg("eax") );
// 		}

// 		this.program.addTest( Program.unlinkedReg("eax") );
// 		this.program.addJE( elseLabel );
// 		this.match(TokenType.RightParen);

// 		this.doBlock(breakToBranch, returnToBranch, false, true, false, returnType);

// 		if (this.isNotEnd() && this.token.type === TokenType.Else) {
// 			const endLabel = this.newBranch();
// 			this.program.addJmp( endLabel );
// 			this.program.addLabel( elseLabel );

// 			this.match(TokenType.Else);

// 			this.doBlock(breakToBranch, returnToBranch, false, true, false, returnType);

// 			this.program.addLabel( endLabel );
// 		}else{
// 			this.program.addLabel( elseLabel );
// 		}

// 		this.program.addCodeLine(null);
// 	}

// 	doWhile(returnToBranch, returnType){
// 		const loopLabel = this.newBranch();
// 		const endLabel = this.newBranch();


// 		this.match(TokenType.While);

// 		this.program.addLabel( loopLabel );

// 		this.match(TokenType.LeftParen);
// 		let type=this.doExpression();
// 		if (type!==IdentityType.Bool){
// 			this.program.addToBool( Program.unlinkedReg("eax") );
// 		}
// 		this.program.addTest( Program.unlinkedReg("eax") );
// 		this.program.addJE( endLabel );
// 		this.match(TokenType.RightParen);

// 		this.doBlock(endLabel, returnToBranch, false, true, false, returnType);

// 		this.program.addJmp( loopLabel );
// 		this.program.addLabel( endLabel );
// 		this.program.addCodeLine(null);
// 	}

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

// 	doLoop(returnToBranch, returnType){
// 		const loopLabel = this.newBranch();
// 		const endLabel = this.newBranch();

// 		this.program.addLabel( loopLabel );

// 		this.match(TokenType.Loop);

// 		this.doBlock(endLabel, returnToBranch, false, true, false, returnType);//{ block }

// 		this.match(TokenType.While);
// 		this.match(TokenType.LeftParen);

// 		let type=this.doExpression();
// 		if (type!==IdentityType.Bool){
// 			this.program.addToBool( Program.unlinkedReg("eax") );
// 		}

// 		this.program.addTest( Program.unlinkedReg("eax") );
// 		this.program.addJNE( loopLabel );
// 		this.program.addLabel( endLabel );

// 		this.match(TokenType.RightParen);
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