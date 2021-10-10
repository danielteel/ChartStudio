#pragma once
#include <vector>
#include <string>

using namespace std;

enum class InterpreterTokenType {
	EndOfFile,
	LineDelim,
	NewLine,
	Void,
	Double,
	String,
	Bool,
	DoubleLiteral,
	StringLiteral,
	BoolLiteral,
	Ident,
	True,
	False,
	Null,
	LeftParen,
	RightParen,
	LeftSquare,
	RightSquare,
	Comma,
	Dot,
	Not,
	And,
	Or,
	Plus,
	Minus,
	Divide,
	Multiply,
	Mod,
	Exponent,
	Question,
	Colon,
	Assignment,
	Equals,
	NotEquals,
	Lesser,
	Greater,
	LesserEquals,
	GreaterEquals,
	Min,
	Max,
	Abs,
	Clamp,
	Floor,
	Ceil,
	LCase,
	UCase,
	Trim,
	Len,
	SubStr,
	While,
	For,
	Loop,
	If,
	Else,
	Break,
	LeftCurly,
	RightCurly,
	Return,
	Exit
};

class TokenObj {
public:
	InterpreterTokenType type;
	string sValue;
	double dValue=0.0;
	size_t line;

	TokenObj(InterpreterTokenType type, string value, int line) {
		this->type = type;
		this->sValue = value;
		this->line = line;
	}
	TokenObj(InterpreterTokenType type, double value, int line) {
		this->type = type;
		this->dValue = value;
		this->line = line;
	}

	TokenObj(InterpreterTokenType type, int line) {
		this->type = type;
		this->line = line;
	}

	TokenObj(const TokenObj& a) {
		this->type = a.type;
		this->sValue = a.sValue;
		this->dValue = a.dValue;
		this->line = a.line;
	}
};

class Tokenizer {
public:
	Tokenizer();
	~Tokenizer();

	void tokenize(string code);

	vector<TokenObj> tokens;

	void throwError(string message);

	void addToken(InterpreterTokenType type, string value);
	void addToken(InterpreterTokenType type, double value);
	void addToken(InterpreterTokenType type);

	bool isNotEnd();

	void getChar();

	void skipWhite();

	void doubleLiteral();

	void stringLiteral();

	void ident();

	void next();

	string errorMsg;

	string code;
	size_t lookIndex = 0;
	char look = 0;
	size_t codeEndIndex = 0;
	size_t currentCodeLine = 1;
	string currentLineText;
};

