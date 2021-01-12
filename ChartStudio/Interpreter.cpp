#include "stdafx.h"
#include <math.h>
#include "Interpreter.h"
#include "CChartObject.h"
#include "CChartBase.h"
#include "CLinearChart.h"
#include "CClampChart.h"
#include "CPolyChart.h"
#include "CScript.h"
#include "CInput.h"
#include "CConstant.h"
#include "CTrendChart.h"
#include "ChartProject.h"
#include "helpers.h"

Interpreter::Interpreter() {
	errorCodeLine = 0;
	errorWasDuring = InterpreterError::CODE;
}


Interpreter::~Interpreter() {
	clearStringPool();
	clearVariables();
	clearChartFunctions();
	clearOps();
	setToken(nullptr);
}

string Interpreter::getErrorMessage() {
	return errorString;
}

int Interpreter::getErrorLine() {
	return errorCodeLine;
}

InterpreterError Interpreter::getPhaseOfError() {
	return errorWasDuring;
}

string Interpreter::getCode() {
	return code;
}

string Interpreter::tokenTypeToString(enum TokenType type) {
	switch (type) {
	case TokenType::Constant:
		return "double constant";
	case TokenType::Ident:
		return "identifier";
	case TokenType::LeftParen:
		return "(";
	case TokenType::RightParen:
		return ")";
	case TokenType::Exponent:
		return "^";
	case TokenType::Plus:
		return "+";
	case TokenType::Minus:
		return "-";
	case TokenType::Divide:
		return "/";
	case TokenType::Multiply:
		return "*";
	case TokenType::Assignment:
		return "=";
	case TokenType::Equals:
		return "==";
	case TokenType::NotEquals:
		return "!=";
	case TokenType::Lesser:
		return "<";
	case TokenType::Greater:
		return ">";
	case TokenType::LesserEquals:
		return "<=";
	case TokenType::GreaterEquals:
		return ">=";
	case TokenType::FuncMin:
		return "min";
	case TokenType::FuncMax:
		return "max";
	case TokenType::FuncAbs:
		return "abs";
	case TokenType::FuncClamp:
		return "clamp";
	case TokenType::FuncBoolIsNil:
		return "isnil";
	case TokenType::While:
		return "while";
	case TokenType::If:
		return "if";
	case TokenType::LeftCurly:
		return "{";
	case TokenType::RightCurly:
		return "}";
	case TokenType::Return:
		return "return";
	case TokenType::Else:
		return "else";
	case TokenType::Break:
		return "break";
	case TokenType::LineDelim:
		return ";";
	case TokenType::Not:
		return "!";
	case TokenType::And:
		return "&&";
	case TokenType::Or:
		return "||";
	case TokenType::Comma:
		return ",";
	case TokenType::Double:
		return "double";
	case TokenType::True:
		return "true";
	case TokenType::False:
		return "false";
	case TokenType::Nil:
		return "nil";
	case TokenType::ChartCall:
		return "chartcall";
	case TokenType::This:
		return "this";
	case TokenType::String:
		return "string";
	}
	return "???";
}

///////////// Tokenizer //////////////
bool Interpreter::number() {
	bool hasDec = false;
	bool notDone = true;
	string num;
	while (isNotEnd() && notDone==true) {
		notDone = false;
		if (isDigit(*look)) {
			num += *look;
			notDone = true;
		}
		if (*look == '.' && hasDec == false) {
			hasDec = true;
			num += *look;
			notDone = true;
		}
		if (notDone==true) getChar();
	}

	if (num.size() < 2 && hasDec == true) {
		errorString = "Expected number but found lone decimal.";
		return false;
	}

	setToken(new TConstant(stod(num)));
	return true;
}

bool Interpreter::stringToken() {
	string str;
	getChar();
	while (isNotEnd() && *look!='"') {
		str += *look;
		getChar();
	}
	if (isNotEnd()){
		if (*look != '"') {
			errorString = "Expected string but found end of code.";
			return false;
		}
	}
	getChar();
	setToken(new TString(str));
	return true;
}


void Interpreter::setToken(Token* tok) {
	if (token) {
		delete token;
		token = nullptr;
	}
	token = tok;
}

bool Interpreter::ident() {
	string name;
	bool notDone = true;
	while (isNotEnd() && notDone==true) {
		notDone = false;
		if (isAlpha(*look) || isDigit(*look) || *look == '_' || *look == '.') {
			name += *look;
			notDone = true;
			getChar();
		}
	}
	if (name.length() == 0) {
		errorString = "Expected identifier but got nothing";
		return false;
	}
	stringToLower(name);
	if (name == "if") {
		setToken(new Token(TokenType::If));
	} else if (name == "min") {
		setToken(new Token(TokenType::FuncMin));
	} else if (name == "max") {
		setToken(new Token(TokenType::FuncMax));
	} else if (name == "clamp") {
		setToken(new Token(TokenType::FuncClamp));
	} else if (name == "abs") {
		setToken(new Token(TokenType::FuncAbs));
	} else if (name == "while") {
		setToken(new Token(TokenType::While));
	} else if (name == "double") {
		setToken(new Token(TokenType::Double));
	} else if (name == "return") {
		setToken(new Token(TokenType::Return));
	} else if (name == "else") {
		setToken(new Token(TokenType::Else));
	} else if (name == "break") {
		setToken(new Token(TokenType::Break));
	} else if (name == "isnil") {
		setToken(new Token(TokenType::FuncBoolIsNil));
	} else if (name == "true") {
		setToken(new Token(TokenType::True));
	} else if (name == "false") {
		setToken(new Token(TokenType::False));
	}else if (name=="nil"){
		setToken(new Token(TokenType::Nil));
	} else if (name == "this") {
		setToken(new Token(TokenType::This));
	} else if (name == "msgbox") {
		setToken(new Token(TokenType::MsgBox));
	} else {
		setToken(new TIdent(name));
	}
	return true;
}

bool Interpreter::chartCall() {
	string name;
	bool notDone = true;
	while (isNotEnd() && notDone == true) {
		notDone = false;
		if (isAlpha(*look) || isDigit(*look) || *look == '_' || *look == '.') {
			name += *look;
			notDone = true;
			getChar();
		}
	}
	if (name.length() == 0) {
		errorString = "Expected chart name but got nothing";
		return false;
	}
	stringToLower(name);
	setToken(new TChartCall(name));
	return true;
}

void Interpreter::getChar() {
	if (isNotEnd()) {
		look++;
	}
}

void Interpreter::skipWhite() {
	while (isNotEnd() && isSpace(*look)) {
		if (*look == '\n') {
			errorCodeLine++;
		}
		getChar();
	}
}

bool Interpreter::isNotEnd() {
	return look != codeEnd;
}


bool Interpreter::next() {
	setToken(nullptr);
	skipWhite();
	if (isNotEnd()){
		if (isDigit(*look) || *look=='.') {
			if (!number()) {
				return false;
			}
		} else if (isAlpha(*look) || *look=='_') {
			if (!ident()) {
				return false;
			}
		} else if (*look == '"') {
			if (!stringToken()) {
				return false;
			}
		}else{
			char symbol = *look;
			getChar();
			switch (symbol) {
			case '@':
				if (!chartCall()) {
					return false;
				}
				break;
			case '{':
				setToken(new Token(TokenType::LeftCurly));
				break;
			case '}':
				setToken(new Token(TokenType::RightCurly));
				break;
			case '(':
				setToken(new Token(TokenType::LeftParen));
				break;
			case ')':
				setToken(new Token(TokenType::RightParen));
				break;
			case ';':
				setToken(new Token(TokenType::LineDelim));
				break;
			case '^':
				setToken(new Token(TokenType::Exponent));
				break;
			case '|':
				if (isNotEnd() && *look == '|') {
					getChar();
					setToken(new Token(TokenType::Or));
				} else {
					errorString = "Incomplete OR operator found, OR operators must be of boolean type '||'";
					return false;
				}
				break;
			case '&':
				if (isNotEnd() && *look == '&') {
					getChar();
					setToken(new Token(TokenType::And));
				} else {
					errorString = "Incomplete AND operator found, AND operators must be of boolean type '&&'";
					return false;
				}
				break;
			case '+':
				setToken(new Token(TokenType::Plus));
				break;
			case '-':
				setToken(new Token(TokenType::Minus));
				break;
			case '/':
				if (isNotEnd() && *look == '/') {
					getChar();
					while (isNotEnd() && *look != '\n') {
						getChar();
					}
					return next();
				} else {
					setToken(new Token(TokenType::Divide));
				}
				break;
			case '*':
				setToken(new Token(TokenType::Multiply));
				break;
			case '!':
				if (isNotEnd() && *look == '=') {
					getChar();
					setToken(new Token(TokenType::NotEquals));
				} else {
					setToken(new Token(TokenType::Not));
				}
				break;
			case '=':
				if (isNotEnd() && *look == '=') {
					getChar();
					setToken(new Token(TokenType::Equals));
				} else {
					setToken(new Token(TokenType::Assignment));
				}
				break;
			case '>':
				if (isNotEnd() && *look == '=') {
					getChar();
					setToken(new Token(TokenType::GreaterEquals));
				} else {
					setToken(new Token(TokenType::Greater));
				}
				break;
			case '<':
				if (isNotEnd() && *look == '=') {
					getChar();
					setToken(new Token(TokenType::LesserEquals));
				} else {
					setToken(new Token(TokenType::Lesser));
				}
				break;
			case ',':
				setToken(new Token(TokenType::Comma));
				break;
			default:
				errorString = "Unexpected symbol found, ";
				errorString += symbol;
				return false;
			}
		}
	}
	return true;
}
/////////////////// End Tokenizer ///////////////
bool Interpreter::match(enum TokenType type) {
	if (token) {
		if (token->type == type) {
			return next();
		}
	}
	string thisToken="nulltoken";
	if (token) thisToken = tokenTypeToString(token->type);
	errorString = "Expected token type "+ tokenTypeToString(type) + " but found "+thisToken+" instead";
	return false;
}

BranchID Interpreter::newBranch() {
	return ++branchCounter;
}

bool Interpreter::isPowerOp() {
	if (token && token->type == TokenType::Exponent) {
		return true;
	}
	return false;
}

bool Interpreter::isTermOp() {
	if (token) {
		switch (token->type) {
		case TokenType::Multiply:
		case TokenType::Divide:
			return true;
		}
	}
	return false;
}

bool Interpreter::isAddOp() {
	if (token) {
		switch (token->type) {
		case TokenType::Plus:
		case TokenType::Minus:
			return true;
		}
	}
	return false;
}
bool Interpreter::isCompareOp() {
	if (token) {
		switch (token->type) {
		case TokenType::Lesser:
		case TokenType::LesserEquals:
		case TokenType::Greater:
		case TokenType::GreaterEquals:
		case TokenType::Equals:
		case TokenType::NotEquals:
			return true;
		}
	}
	return false;
}
bool Interpreter::isAndOp() {
	if (token) {
		switch (token->type) {
		case TokenType::And:
			return true;
		}
	}
	return false;
}
bool Interpreter::isOrOp() {
	if (token) {
		switch (token->type) {
		case TokenType::Or:
			return true;
		}
	}
	return false;
}


bool Interpreter::doAbs() {
	BranchID ifPosBranch = newBranch();
	if (!match(TokenType::FuncAbs)) return false;
	if (!match(TokenType::LeftParen)) return false;
	if (!doExpression()) return false;
	addOp(ROperation::newCmpToConstant(errorCodeLine, &eax, 0));
	addOp(ROperation::newJA(errorCodeLine, ifPosBranch));
	addOp(ROperation::newNeg(errorCodeLine, &eax));
	addOp(ROperation::newLabel(errorCodeLine, ifPosBranch));
	if (!match(TokenType::RightParen)) return false;
	return true;
}

bool Interpreter::doMin() {
	BranchID compareBranch = newBranch();
	if (!match(TokenType::FuncMin)) return false;
	if (!match(TokenType::LeftParen)) return false;
	if (!doExpression()) return false;
	addOp(ROperation::newPush(errorCodeLine, &eax));
	if (!match(TokenType::Comma)) return false;
	if (!doExpression()) return false;
	addOp(ROperation::newPop(errorCodeLine, &ebx));
	addOp(ROperation::newCmp(errorCodeLine, &eax, &ebx));
	addOp(ROperation::newJB(errorCodeLine, compareBranch));
	addOp(ROperation::newMov(errorCodeLine, &eax, &ebx));
	addOp(ROperation::newLabel(errorCodeLine, compareBranch));
	if (!match(TokenType::RightParen)) return false;
	return true;
}

bool Interpreter::doMax() {
	BranchID compareBranch = newBranch();
	if (!match(TokenType::FuncMax)) return false;
	if (!match(TokenType::LeftParen)) return false;
	if (!doExpression()) return false;
	addOp(ROperation::newPush(errorCodeLine, &eax));
	if (!match(TokenType::Comma)) return false;
	if (!doExpression()) return false;
	addOp(ROperation::newPop(errorCodeLine, &ebx));
	addOp(ROperation::newCmp(errorCodeLine, &eax, &ebx));
	addOp(ROperation::newJA(errorCodeLine, compareBranch));
	addOp(ROperation::newMov(errorCodeLine, &eax, &ebx));
	addOp(ROperation::newLabel(errorCodeLine, compareBranch));
	if (!match(TokenType::RightParen)) return false;
	return true;
}

bool Interpreter::doClamp() {
	BranchID compareBranch = newBranch();
	BranchID compareBranch2 = newBranch();
	if (!match(TokenType::FuncClamp)) return false;
	if (!match(TokenType::LeftParen)) return false;

	if (!doExpression()) return false;
	addOp(ROperation::newPush(errorCodeLine, &eax));

	if (!match(TokenType::Comma)) return false;

	if (!doExpression()) return false;
	addOp(ROperation::newPop(errorCodeLine, &ebx));
	addOp(ROperation::newCmp(errorCodeLine, &eax, &ebx));
	addOp(ROperation::newJA(errorCodeLine, compareBranch));
	addOp(ROperation::newMov(errorCodeLine, &eax, &ebx));
	addOp(ROperation::newLabel(errorCodeLine, compareBranch));
	addOp(ROperation::newPush(errorCodeLine, &eax));

	if (!match(TokenType::Comma)) return false;

	if (!doExpression()) return false;
	addOp(ROperation::newPop(errorCodeLine, &ebx));
	addOp(ROperation::newCmp(errorCodeLine, &eax, &ebx));
	addOp(ROperation::newJB(errorCodeLine, compareBranch2));
	addOp(ROperation::newMov(errorCodeLine, &eax, &ebx));
	addOp(ROperation::newLabel(errorCodeLine, compareBranch2));

	if (!match(TokenType::RightParen)) return false;
	return true;
}

bool Interpreter::doChartCall() {
	TChartCall* chartCall = reinterpret_cast<TChartCall*>(token);
	if (!chartCall) {
		errorString = "Null chart call";
		return false;
	}

	ChartFunction* chartFunction = getChartFunction(chartCall->name);
	if (!chartFunction) {
		errorString = "Chart doesnt exist '" + chartCall->name + "'";
		return false;
	}

	string chartCallName = chartCall->name;

	chartCall = nullptr;

	bool isTrend = chartFunction->chartBase->type == ChartObjectType::TrendChart;
	bool isLinear = chartFunction->chartBase->type == ChartObjectType::LinearChart;
	bool isPoly = chartFunction->chartBase->type == ChartObjectType::PolyChart;
	bool isClamp = chartFunction->chartBase->type == ChartObjectType::ClampChart;

	int inputCount = 2;
	if (isTrend) {
		inputCount = 3;
	}

	if (!match(TokenType::ChartCall)) return false;
	if (!match(TokenType::LeftParen)) return false;
	for (int i = 0; i < inputCount; i++) {
		if (!doExpression()) return false;
		if (i < inputCount - 1) {
			addOp(ROperation::newPush(errorCodeLine, &eax));
			if (!match(TokenType::Comma)) return false;
		}
	}
	if (!match(TokenType::RightParen)) return false;

	addOp(ROperation::newPop(errorCodeLine, &ebx));
	if (isTrend) {
		addOp(ROperation::newPop(errorCodeLine, &ecx));
		addOp(ROperation::newTrendChartCall(errorCodeLine, chartFunction->chartBase, &ecx, &ebx, &eax));
	} else {
		if (isLinear) {
			addOp(ROperation::newLinearChartCall(errorCodeLine, chartFunction->chartBase, &ebx, &eax));
		}
		if (isPoly) {
			addOp(ROperation::newPolyChartCall(errorCodeLine, chartFunction->chartBase, &ebx, &eax));
		}
		if (isClamp) {
			addOp(ROperation::newClampChartCall(errorCodeLine, chartFunction->chartBase, &ebx, &eax));
		}
	}
	return true;
}


bool Interpreter::doFactor() {
	if (token) {
		switch (token->type) {
		case TokenType::Nil:
			addOp(ROperation::newLoad(errorCodeLine, &eax, nullopt));
			return match(TokenType::Nil);

		case TokenType::This:
			addOp(ROperation::newGetThis(errorCodeLine));
			return match(TokenType::This);

		case TokenType::Constant:
			{
				TConstant* constant = reinterpret_cast<TConstant*>(token);
				if (constant) {
					addOp(ROperation::newLoad(errorCodeLine, &eax, constant->value));
				} else {
					errorString = "Null constant";
					return false;
				}
			}
			return match(TokenType::Constant);

		case TokenType::Ident:
			{
				TIdent* ident = reinterpret_cast<TIdent*>(token);
				if (ident) {
					Variable* variable = getVariable(ident->name);
					if (variable) {
						addOp(ROperation::newMov(errorCodeLine, &eax, &variable->value));
					} else {
						errorString = "Variable doesnt exist '" + ident->name + "'";
						return false;
					}
				} else {
					errorString = "Null identifier";
					return false;
				}
			}
			return match(TokenType::Ident);

		case TokenType::ChartCall:
			return doChartCall();

		case TokenType::LeftParen:
			if (!match(TokenType::LeftParen)) return false;
			if (!doExpression()) return false;
			if (!match(TokenType::RightParen)) return false;
			return true;

		case TokenType::Minus:
			if (!match(TokenType::Minus)) return false;
			if (token) {
				if (!doFactor()) return false;
				addOp(ROperation::newNeg(errorCodeLine, &eax));
				return true;
			}
			errorString = "Unary minus expects a factor";
			return false;

		case TokenType::True:
			addOp(ROperation::newSet(errorCodeLine, &eax));
			return match(TokenType::True);

		case TokenType::False:
			addOp(ROperation::newClear(errorCodeLine, &eax));
			return match(TokenType::False);

		case TokenType::Not:
			if (!match(TokenType::Not)) return false;
			if (token) {
				if (!doFactor()) return false;
				addOp(ROperation::newNotOp(errorCodeLine, &eax));
				return true;
			}
			errorString = "Unary not expects a factor";
			return false;

		case TokenType::FuncBoolIsNil:
			{
				if (!match(TokenType::FuncBoolIsNil)) return false;
				if (!match(TokenType::LeftParen)) return false;
				TIdent* ident = reinterpret_cast<TIdent*>(token);
				if (ident == nullptr) {
					errorString = "IsNil ident is null";
					return false;
				}
				if (token->type != TokenType::Ident) {
					errorString = "IsNil expects an identifier but found " + (token != nullptr ? tokenTypeToString(token->type) : "nulltoken");
					return false;
				}
				Variable* variable = getVariable(ident->name);
				if (variable == nullptr) {
					errorString = ident->name + " variable doesnt exist!";
					return false;
				}
				addOp(ROperation::newIsNil(errorCodeLine, &eax, &variable->value));
				if (!match(TokenType::Ident)) return false;
				if (!match(TokenType::RightParen)) return false;
			}
			return true;

		case TokenType::FuncAbs:
			return doAbs();
		case TokenType::FuncClamp:
			return doClamp();
		case TokenType::FuncMax:
			return doMax();
		case TokenType::FuncMin:
			return doMin();
		}
	}
	errorString = "Expected factor but found "+ (token!=nullptr ? tokenTypeToString(token->type) : "nulltoken");
	return false;
}


bool Interpreter::doPower() {
	if (!doFactor()) return false;
	while (isPowerOp()) {
		addOp(ROperation::newPush(errorCodeLine, &eax));
		switch (token->type) {
		case TokenType::Exponent:
			if (!match(TokenType::Exponent)) return false;
			if (!doFactor()) return false;
			addOp(ROperation::newMov(errorCodeLine, &ebx, &eax));
			addOp(ROperation::newPop(errorCodeLine, &eax));
			addOp(ROperation::newPower(errorCodeLine, &eax, &ebx));
			break;
		}
	}
	return true;
}

bool Interpreter::doTerm() {
	if (!doPower()) return false;
	while (isTermOp()) {
		addOp(ROperation::newPush(errorCodeLine, &eax));
		switch (token->type) {
		case TokenType::Multiply:
			if (!match(TokenType::Multiply)) return false;
			if (!doPower()) return false;
			addOp(ROperation::newPop(errorCodeLine, &ebx));
			addOp(ROperation::newMul(errorCodeLine, &eax, &ebx));
			break;
		case TokenType::Divide:
			if (!match(TokenType::Divide)) return false;
			if (!doPower()) return false;
			addOp(ROperation::newMov(errorCodeLine, &ebx, &eax));
			addOp(ROperation::newPop(errorCodeLine, &eax));
			addOp(ROperation::newDiv(errorCodeLine, &eax, &ebx));
			break;
		}
	}
	return true;
}

bool Interpreter::doAdd() {
	if (!doTerm()) return false;
	while (isAddOp()) {
		addOp(ROperation::newPush(errorCodeLine, &eax));
		switch (token->type) {
		case TokenType::Plus:
			if (!match(TokenType::Plus)) return false;
			if (!doTerm()) return false;
			addOp(ROperation::newPop(errorCodeLine, &ebx));
			addOp(ROperation::newAdd(errorCodeLine, &eax, &ebx));
			break;
		case TokenType::Minus:
			if (!match(TokenType::Minus)) return false;
			if (!doTerm()) return false;
			addOp(ROperation::newPop(errorCodeLine, &ebx));
			addOp(ROperation::newSub(errorCodeLine, &eax,&ebx));
			addOp(ROperation::newNeg(errorCodeLine, &eax));
			break;
		}
	}
	return true;
}

bool Interpreter::doCompare() {
	if (!doAdd()) return false;
	while (isCompareOp()) {
		addOp(ROperation::newPush(errorCodeLine, &eax));
		switch (token->type) {
		case TokenType::Equals:
			if (!match(TokenType::Equals)) return false;
			if (!doAdd()) return false;
			addOp(ROperation::newPop(errorCodeLine, &ebx));
			addOp(ROperation::newCmp(errorCodeLine, &ebx, &eax));
			addOp(ROperation::newSE(errorCodeLine, &eax));
			break;
		case TokenType::NotEquals:
			if (!match(TokenType::NotEquals)) return false;
			if (!doAdd()) return false;
			addOp(ROperation::newPop(errorCodeLine, &ebx));
			addOp(ROperation::newCmp(errorCodeLine, &ebx, &eax));
			addOp(ROperation::newSNE(errorCodeLine, &eax));
			break;
		case TokenType::Greater:
			if (!match(TokenType::Greater)) return false;
			if (!doAdd()) return false;
			addOp(ROperation::newPop(errorCodeLine, &ebx));
			addOp(ROperation::newCmp(errorCodeLine, &ebx, &eax));
			addOp(ROperation::newSA(errorCodeLine, &eax));
			break;
		case TokenType::GreaterEquals:
			if (!match(TokenType::GreaterEquals)) return false;
			if (!doAdd()) return false;
			addOp(ROperation::newPop(errorCodeLine, &ebx));
			addOp(ROperation::newCmp(errorCodeLine, &ebx, &eax));
			addOp(ROperation::newSAE(errorCodeLine, &eax));
			break;
		case TokenType::Lesser:
			if (!match(TokenType::Lesser)) return false;
			if (!doAdd()) return false;
			addOp(ROperation::newPop(errorCodeLine, &ebx));
			addOp(ROperation::newCmp(errorCodeLine, &ebx, &eax));
			addOp(ROperation::newSB(errorCodeLine, &eax));
			break;
		case TokenType::LesserEquals:
			if (!match(TokenType::LesserEquals)) return false;
			if (!doAdd()) return false;
			addOp(ROperation::newPop(errorCodeLine, &ebx));
			addOp(ROperation::newCmp(errorCodeLine, &ebx, &eax));
			addOp(ROperation::newSBE(errorCodeLine, &eax));
			break;
		}
	}
	return true;
}


bool Interpreter::doAnd() {
	if (!doCompare()) return false;
	while (isAndOp()) {
		addOp(ROperation::newPush(errorCodeLine, &eax));
		switch (token->type) {
		case TokenType::And:
			if (!match(TokenType::And)) return false;
			if (!doCompare()) return false;
			addOp(ROperation::newPop(errorCodeLine, &ebx));
			addOp(ROperation::newAnd(errorCodeLine, &eax, &ebx));
			break;
		}
	}
	return true;
}

bool Interpreter::doExpression() {
	if (!doAnd()) return false;
	while (isOrOp()) {
		addOp(ROperation::newPush(errorCodeLine, &eax));
		switch (token->type) {
		case TokenType::Or:
			if (!match(TokenType::Or)) return false;
			if (!doAnd()) return false;
			addOp(ROperation::newPop(errorCodeLine, &ebx));
			addOp(ROperation::newOr(errorCodeLine, &eax, &ebx));
			break;
		}
	}
	return true;
}


bool Interpreter::doIf(optional<BranchID> breakToBranch) {
	BranchID elseLabel = newBranch();
	BranchID endLabel = newBranch();

	if (!match(TokenType::If)) return false;
	if (!doExpression()) return false;
	
	addOp(ROperation::newTest(errorCodeLine, &eax));
	addOp(ROperation::newJE(errorCodeLine, elseLabel));

	if (!match(TokenType::LeftCurly)) return false;
	if (!doBlock(breakToBranch, true)) return false;
	if (!match(TokenType::RightCurly)) return false;

	addOp(ROperation::newJmp(errorCodeLine, endLabel));
	addOp(ROperation::newLabel(errorCodeLine, elseLabel));

	if (token && token->type == TokenType::Else) {
		if (!match(TokenType::Else)) return false;
		if (token && token->type == TokenType::If) {
			if (!doIf(breakToBranch)) return false;
		} else {
			if (!match(TokenType::LeftCurly)) return false;
			if (!doBlock(breakToBranch, true)) return false;
			if (!match(TokenType::RightCurly)) return false;
		}
	}
	addOp(ROperation::newLabel(errorCodeLine, endLabel));
	return true;
}

bool Interpreter::doWhile(optional<BranchID> breakToBranch) {
	BranchID loopLabel = newBranch();
	BranchID endLabel = newBranch();

	if (!match(TokenType::While)) return false;
	
	addOp(ROperation::newLabel(errorCodeLine, loopLabel));
	if (!doExpression()) return false;
	addOp(ROperation::newTest(errorCodeLine, &eax));
	addOp(ROperation::newJE(errorCodeLine, endLabel));

	if (!match(TokenType::LeftCurly)) return false;
	if (!doBlock(endLabel, true)) return false;
	if (!match(TokenType::RightCurly)) return false;

	addOp(ROperation::newJmp(errorCodeLine, loopLabel));
	addOp(ROperation::newLabel(errorCodeLine, endLabel));

	return true;
}

bool Interpreter::doBreak(optional<BranchID> breakToBranch) {
	if (match(TokenType::Break)) {
		
		if (breakToBranch == nullopt) {
			errorString = "No loop to 'break' from.";
			return false;
		}

		addOp(ROperation::newJmp(errorCodeLine, *breakToBranch));

		return (match(TokenType::LineDelim));
	}
	return false;
}

bool Interpreter::doReturn() {
	if (match(TokenType::Return)) {
		if (!doExpression()) return false;
		
		addOp(ROperation::newRet(errorCodeLine));

		return (match(TokenType::LineDelim));
	}
	return false;
}

bool Interpreter::doDouble() {
	if (match(TokenType::Double)) {
		if (token && token->type == TokenType::Ident) {
			bool notDone = true;
			while (notDone) {
				notDone = false;
				TIdent* ident = reinterpret_cast<TIdent*>(token);
				if (!addVariable(ident->name, false, nullopt)) return false;
				string identName = ident->name;
				if (!match(TokenType::Ident)) return false;
				if (token && token->type == TokenType::Assignment) {
					if (!doAssignmentWithName(identName)) return false;
				}
				if (token && token->type == TokenType::Comma) {
					if (!match(TokenType::Comma)) return false;
					notDone = true;
				}
			}
			return (match(TokenType::LineDelim));
		} else {
			errorString = "Expected idenfitifer but found "+ (token != nullptr ? tokenTypeToString(token->type) : "nulltoken");
			return false;
		}
	}
	return false;
}

bool Interpreter::doAssignmentWithName(string identName) {
	Variable* variable = getVariable(identName);
	if (!variable) {
		errorString = identName + " variable doesnt exist!";
		return false;
	}
	if (variable->isConstant) {
		errorString = identName + " is a constant, you cant assign anything to it!";
		return false;
	}
	if (!match(TokenType::Assignment)) return false;

	if (!doExpression()) return false;

	addOp(ROperation::newMov(errorCodeLine, &variable->value, &eax));

	return true;
}

bool Interpreter::doAssignment() {
	TIdent* ident = reinterpret_cast<TIdent*>(token);
	if (!ident) {
		errorString = "Null ident token";
		return false;
	}
	string identName = ident->name;
	ident = nullptr;

	if (match(TokenType::Ident)) {
		if (!doAssignmentWithName(identName)) return false;
		return (match(TokenType::LineDelim));
	}
	return false;
}

bool Interpreter::doBlock(optional<BranchID> breakToBranch, bool ifWantsRightCurly) {
	while (token) {
		switch (token->type) {
		case TokenType::If:
			if (!doIf(breakToBranch)) return false;
			break;

		case TokenType::While:
			if (!doWhile(breakToBranch)) return false;
			break;

		case TokenType::Break:
			if (!doBreak(breakToBranch)) return false;
			break;

		case TokenType::Return:
			if (!doReturn()) return false;
			break;

		case TokenType::Double:
			if (!doDouble()) return false;
			break;

		case TokenType::MsgBox:
			{
				if (!match(TokenType::MsgBox)) return false;
				if (token->type != TokenType::String) {
					match(TokenType::String);
					return false;
				}
				TString* strToken = reinterpret_cast<TString*>(token);
				if (strToken) {
					string* str = newPoolString(strToken->str);
					if (str) {
						addOp(ROperation::newMsgBox(errorCodeLine, str));
					} else {
						errorString = "Error adding string to pool";
						return false;
					}
				} else {
					errorString = "Error adding string to pool";
					return false;
				}
				if (!match(TokenType::String)) return false;
				if (!match(TokenType::LineDelim)) return false;
			}
			break;

		case TokenType::This:
			if (!match(TokenType::This)) return false;
			if (!match(TokenType::Assignment)) return false;
			if (!doExpression()) return false;
			addOp(ROperation::newSetThis(errorCodeLine));
			if (!match(TokenType::LineDelim)) return false;
			break;

		case TokenType::Ident:
			if (!doAssignment()) return false;
			break;

		case TokenType::LineDelim:
			if (!match(TokenType::LineDelim)) return false;
			break;

		case TokenType::RightCurly:
			if (ifWantsRightCurly) return true;
			errorString = "Unexpected symbol found '}'";
			return false;

		default:
			errorString = "Unexpected token in block";
			return false;
		}
	}
	if (ifWantsRightCurly) {
		errorString = "Expected '}' but found "+ (token != nullptr ? tokenTypeToString(token->type) : "nulltoken");
		return false;
	}
	return true;
}

void Interpreter::addOp(ROperation * rop) {
	compiledProgram.push_back(rop);
}

void Interpreter::clearOps() {
	for (size_t i = 0; i < compiledProgram.size(); i++) {
		if (compiledProgram[i]) {
			delete compiledProgram[i];
			compiledProgram[i] = nullptr;
		}
	}
	compiledProgram.clear();
}

void Interpreter::clearStringPool() {
	for (auto & str : stringPool) {
		delete str;
		str = nullptr;
	}
	stringPool.clear();
}

string* Interpreter::newPoolString(string str) {
	string* newstr = new string(str);
	stringPool.push_back(newstr);
	return newstr;
}

bool Interpreter::compile(ChartProject* chartProject, CChartObject* thisChartObject) {
	updateChartFunctions(chartProject);
	updateVariables(chartProject, thisChartObject);

	clearStringPool();
	clearOps();

	branchCounter = 0;

	errorWasDuring = InterpreterError::CODE;
	errorCodeLine = 1;
	errorString = "";
	look = code.begin();
	codeEnd = code.end();

	if (next()) {
		if (!doBlock(nullopt, false)) return false;
		return true;
	}
	return false;
}

Variable* Interpreter::getVariable(string name) {
	stringToLower(name);
	if (variables.size() > 0) {
		for (size_t i = 0; i < variables.size(); i++) {
			if (variables[i]) {
				if (variables[i]->name == name) {
					return variables[i];
				}
			}
		}
	}
	return nullptr;
}

bool Interpreter::addVariable(string name, bool isConstant, optional<double> value) {
	stringToLower(name);
	if (!getVariable(name)) {
		variables.push_back(new Variable(name, isConstant, value));
		return true;
	}
	errorString = "Duplicate variable name";
	return false;
}

void Interpreter::updateVariables(ChartProject* chartProject, CChartObject* thisChartObject) {
	clearVariables();

	if (chartProject) {
		for (size_t i = 0; i < chartProject->constants.size(); i++) {
			addVariable(chartProject->constants[i]->getName(), true, chartProject->constants[i]->result);
		}
		if (thisChartObject && thisChartObject->type != ChartObjectType::Input) {
			for (size_t i = 0; i < chartProject->inputs.size(); i++) {
				addVariable(chartProject->inputs[i]->getName(), true, chartProject->inputs[i]->result);
			}
			for (size_t i = 0; i < chartProject->objects.size(); i++) {
				if (chartProject->objects[i] == thisChartObject) {
					break;
				}
				addVariable(chartProject->objects[i]->getName(), true, chartProject->objects[i]->result);
			}
		}
	}
}

void Interpreter::clearVariables() {
	for (size_t i = 0; i < variables.size(); i++) {
		if (variables[i]) {
			delete variables[i];
			variables[i] = nullptr;
		}
	}
	variables.clear();
}

ChartFunction* Interpreter::getChartFunction(string name){
	stringToLower(name);
	for (auto & chartFunction : chartFunctions) {
		if (chartFunction->name == name) {
			return chartFunction;
		}
	}
	return nullptr;
}

void Interpreter::updateChartFunctions(ChartProject * chartProject) {
	clearChartFunctions();
	if (chartProject) {
		for (auto& object : chartProject->objects) {
			if (object->toChartBase()) {
				CChartBase* chartBase = reinterpret_cast<CChartBase*>(object);
				if (chartBase) {
					chartFunctions.push_back(new ChartFunction(stringToLowerCopy(chartBase->getName()), chartBase));
				}
			}
		}
	}
}

void Interpreter::clearChartFunctions() {
	for (auto& chartFunction : chartFunctions) {
		delete chartFunction;
		chartFunction = nullptr;
	}
	chartFunctions.clear();
}

bool Interpreter::getLabelIterator(BranchID branch, vector<ROperation*>::iterator& returnIterator) {
	for (size_t i = 0; i < compiledProgram.size(); i++) {
		if (compiledProgram[i]->type == ROp::label) {
			if (compiledProgram[i]->thisLabelID == branch) {
				vector<ROperation*>::iterator jmpIterator = compiledProgram.begin();
				advance(jmpIterator, i);
				returnIterator = jmpIterator;
				return true;
			}
		}
	}
	return false;
}

bool Interpreter::link() {
	errorWasDuring = InterpreterError::LINK;
	for (size_t i = 0; i < compiledProgram.size(); i++) {
		switch (compiledProgram[i]->type) {
		case ROp::jmp:
		case ROp::ja:
		case ROp::jae:
		case ROp::jb:
		case ROp::jbe:
		case ROp::je:
		case ROp::jne:
			{
				vector<ROperation*>::iterator jmpToIterator;
				if (!getLabelIterator(compiledProgram[i]->toLabelID, jmpToIterator)) {
					errorString = "Unable to resolve label reference " + to_string(compiledProgram[i]->toLabelID);
					return false;
				}
				compiledProgram[i]->toLabel = jmpToIterator;
			}
		}
	}
	return true;
}
bool Interpreter::execute(ChartProject* chartProject, optional<double>& returnedValue) {
	errorWasDuring = InterpreterError::EXECUTION;

	if (compiledProgram.empty()) {
		errorString = "Empty program, it must have at least a return.";
		return false;
	}

	//reset 'machine' state
	flag_AGreater = false;
	flag_BGreater = false;
	flag_Equals = false;
	eax = nullopt;
	ebx = nullopt;
	ecx = nullopt;
	while (runStack.size() > 0) runStack.pop();

	instructionPointer = compiledProgram.begin();
	vector<ROperation*>::iterator endOfProgram = compiledProgram.end();

	while (instructionPointer != endOfProgram) {
		errorCodeLine = (*instructionPointer)->codeLine;
		switch ((*instructionPointer)->type) {
		case ROp::jmp:
			instructionPointer = (*instructionPointer)->toLabel;
			break;
		case ROp::je:
			if (flag_Equals) instructionPointer = (*instructionPointer)->toLabel;
			break;
		case ROp::jne:
			if (!flag_Equals) instructionPointer = (*instructionPointer)->toLabel;
			break;
		case ROp::ja:
			if (!flag_Equals && flag_AGreater) instructionPointer = (*instructionPointer)->toLabel;
			break;
		case ROp::jae:
			if (flag_Equals || flag_AGreater) instructionPointer = (*instructionPointer)->toLabel;
			break;
		case ROp::jb:
			if (!flag_Equals && flag_BGreater) instructionPointer = (*instructionPointer)->toLabel;
			break;
		case ROp::jbe:
			if (flag_Equals || flag_BGreater) instructionPointer = (*instructionPointer)->toLabel;
			break;
		case ROp::push:
			runStack.push(*((*instructionPointer)->double1));
			break;
		case ROp::pop:
			*((*instructionPointer)->double1) = runStack.top();
			runStack.pop();
			break;
		case ROp::mov:
			*((*instructionPointer)->double1) = *((*instructionPointer)->double2);
			break;
		case ROp::load:
			*((*instructionPointer)->double1) = (*instructionPointer)->constant;
			break;
		case ROp::add:
			if ((!*(*instructionPointer)->double1) || !*(*instructionPointer)->double2){
				errorString = "Trying to access double from nil optional";
				return false;
			}
			*((*instructionPointer)->double1) = ((*instructionPointer)->double1)->value() + ((*instructionPointer)->double2)->value();
			break;
		case ROp::sub:
			if ((!*(*instructionPointer)->double1) || !*(*instructionPointer)->double2) {
				errorString = "Trying to access double from nil optional";
				return false;
			}
			*((*instructionPointer)->double1) = ((*instructionPointer)->double1)->value() - ((*instructionPointer)->double2)->value();
			break;
		case ROp::div:
			if ((!*(*instructionPointer)->double1) || !*(*instructionPointer)->double2) {
				errorString = "Trying to access double from nil optional";
				return false;
			}
			if (compare_float(0, ((*instructionPointer)->double2)->value())) {
				errorString = "Divide by zero";
				return false;
			}
			*((*instructionPointer)->double1) = ((*instructionPointer)->double1)->value() / ((*instructionPointer)->double2)->value();
			break;
		case ROp::mul:
			if ((!*(*instructionPointer)->double1) || !*(*instructionPointer)->double2) {
				errorString = "Trying to access double from nil optional";
				return false;
			}
			*((*instructionPointer)->double1) = ((*instructionPointer)->double1)->value() * ((*instructionPointer)->double2)->value();
			break;
		case ROp::neg:
			if ((!*(*instructionPointer)->double1)) {
				errorString = "Trying to access double from nil optional";
				return false;
			}
			*((*instructionPointer)->double1) = 0 - ((*instructionPointer)->double1)->value();
			break;
		case ROp::power:
			if ((!*(*instructionPointer)->double1) || !*(*instructionPointer)->double2) {
				errorString = "Trying to access double from nil optional";
				return false;
			}
			*((*instructionPointer)->double1) = pow(((*instructionPointer)->double1)->value(), ((*instructionPointer)->double2)->value());
			break;
		case ROp::andop:
			if ((!*(*instructionPointer)->double1) || !*(*instructionPointer)->double2) {
				errorString = "Trying to access double from nil optional";
				return false;
			}
			if (abs(((*instructionPointer)->double1)->value()) >= 0.5 && abs(((*instructionPointer)->double2)->value()) >= 0.5) {
				*((*instructionPointer)->double1) = 1.0f;
			} else {
				*((*instructionPointer)->double1) = 0.0f;
			}
			break;
		case ROp::orop:
			if ((!*(*instructionPointer)->double1) || !*(*instructionPointer)->double2) {
				errorString = "Trying to access double from nil optional";
				return false;
			}
			if (abs(((*instructionPointer)->double1)->value()) >= 0.5 || abs(((*instructionPointer)->double2)->value()) >= 0.5) {
				*((*instructionPointer)->double1) = 1.0f;
			} else {
				*((*instructionPointer)->double1) = 0.0f;
			}
			break;
		case ROp::notop:
			if ((!*(*instructionPointer)->double1)) {
				errorString = "Trying to access double from nil optional";
				return false;
			}
			if (abs(((*instructionPointer)->double1)->value()) >= 0.5) {
				*((*instructionPointer)->double1) = 0.0f;
			} else {
				*((*instructionPointer)->double1) = 1.0f;
			}
			break;
		case ROp::clear:
			*((*instructionPointer)->double1) = 0.0f;
			break;
		case ROp::set:
			*((*instructionPointer)->double1) = 1.0f;
			break;
		case ROp::cmp:
			if ((!*(*instructionPointer)->double1) || !*(*instructionPointer)->double2) {
				errorString = "Trying to access double from nil optional";
				return false;
			}
			if (*((*instructionPointer)->double1) > *((*instructionPointer)->double2)){
				flag_AGreater = true;
			} else {
				flag_AGreater = false;
			}
			if (*((*instructionPointer)->double1) < *((*instructionPointer)->double2)) {
				flag_BGreater = true;
			} else {
				flag_BGreater = false;
			}
			if (compare_float(((*instructionPointer)->double1)->value(), ((*instructionPointer)->double2)->value())) {
				flag_Equals = true;
			} else {
				flag_Equals = false;
			}
			break;
		case ROp::cmptoconstant:
			if ((!*(*instructionPointer)->double1) || !(*instructionPointer)->constant) {
				errorString = "Trying to access double from nil optional";
				return false;
			}
			if (*((*instructionPointer)->double1) > *((*instructionPointer)->constant)) {
				flag_AGreater = true;
			} else {
				flag_AGreater = false;
			}
			if (*((*instructionPointer)->double1) < *((*instructionPointer)->constant)) {
				flag_BGreater = true;
			} else {
				flag_BGreater = false;
			}
			if (compare_float(((*instructionPointer)->double1)->value(), *((*instructionPointer)->constant))) {
				flag_Equals = true;
			} else {
				flag_Equals = false;
			}
			break;

		case ROp::test:
			if ((!*(*instructionPointer)->double1)) {
				errorString = "Trying to access double from nil optional";
				return false;
			}
			if (abs(((*instructionPointer)->double1)->value()) < 0.5f) {
				flag_Equals = true;
			} else {
				flag_Equals = false;
			}
			break;
		case ROp::label:
			//essentially a NOP
			break;
		case ROp::se:
			if (flag_Equals) {
				*((*instructionPointer)->double1) = 1.0f;
			} else {
				*((*instructionPointer)->double1) = 0.0f;
			}
			break;
		case ROp::sne:
			if (!flag_Equals) {
				*((*instructionPointer)->double1) = 1.0f;
			} else {
				*((*instructionPointer)->double1) = 0.0f;
			}
			break;
		case ROp::sa:
			if (flag_AGreater) {
				*((*instructionPointer)->double1) = 1.0f;
			} else {
				*((*instructionPointer)->double1) = 0.0f;
			}
			break;
		case ROp::sae:
			if (flag_AGreater || flag_Equals) {
				*((*instructionPointer)->double1) = 1.0f;
			} else {
				*((*instructionPointer)->double1) = 0.0f;
			}
			break;
		case ROp::sb:
			if (flag_BGreater) {
				*((*instructionPointer)->double1) = 1.0f;
			} else {
				*((*instructionPointer)->double1) = 0.0f;
			}
			break;
		case ROp::sbe:
			if (flag_BGreater || flag_Equals) {
				*((*instructionPointer)->double1) = 1.0f;
			} else {
				*((*instructionPointer)->double1) = 0.0f;
			}
			break;
		case ROp::ret:
			returnedValue = eax;
			return true;
		case ROp::isnil:
			if ((!*(*instructionPointer)->double2)) {
				*((*instructionPointer)->double1) = 1.0f;
			} else {
				*((*instructionPointer)->double1) = 0.0f;
			}
			break;
		case ROp::callclamp:
			{
				CClampChart* clampChart = reinterpret_cast<CClampChart*>((*instructionPointer)->chartBase);
				*(*instructionPointer)->double2=clampChart->figureClamp(*(*instructionPointer)->double1, *(*instructionPointer)->double2);
			}
			break;
		case ROp::calllinear:
			{
				CLinearChart* linearChart = reinterpret_cast<CLinearChart*>((*instructionPointer)->chartBase);
				*(*instructionPointer)->double2 = linearChart->figureChart(*(*instructionPointer)->double1, *(*instructionPointer)->double2);
			}
			break;
		case ROp::callpoly:
			{
				CPolyChart* polyChart = reinterpret_cast<CPolyChart*>((*instructionPointer)->chartBase);
				*(*instructionPointer)->double2 = polyChart->hitTestPoly(*(*instructionPointer)->double1, *(*instructionPointer)->double2);
			}
			break;
		case ROp::calltrend:
			{
				CTrendChart* trendChart = reinterpret_cast<CTrendChart*>((*instructionPointer)->chartBase);
				*(*instructionPointer)->double3 = trendChart->figureTrend(*(*instructionPointer)->double1, *(*instructionPointer)->double2, *(*instructionPointer)->double3);
			}
			break;
		case ROp::setThis:
			returnedValue = eax;
			break;
		case ROp::getThis:
			eax = returnedValue;
			break;
		case ROp::msgbox:
			MessageBoxA(GetActiveWindow(), ((*instructionPointer)->string1)->c_str(), "Alert", 0);
		}

		instructionPointer++;
	}

	return true;
}

bool Interpreter::checkCompile(ChartProject * chartProject, CChartObject * thisChartObject, string code, int & errorLine, string & errorMessage) {
	this->code = code;
	
	if (compile(chartProject, thisChartObject)) {
		return true;
	}

	errorLine = this->errorCodeLine;
	errorMessage = this->errorString;
	return false;
}

bool Interpreter::runCode(ChartProject* chartProject, CChartObject* thisChartObject, string theCode, optional<double>& returnedValue) {
	code = theCode;

	if (thisChartObject) {
		if (thisChartObject->type != ChartObjectType::Input) {
			returnedValue = nullopt;
		}
	}

	if (compile(chartProject, thisChartObject)) {
		if (link()) {
			return execute(chartProject, returnedValue);
		}
	}
	return false;
}