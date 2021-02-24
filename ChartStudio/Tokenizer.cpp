#include "stdafx.h"
#include "Tokenizer.h"
#include "helpers.h"


Tokenizer::Tokenizer() {
}


Tokenizer::~Tokenizer() {
}


void Tokenizer::tokenize(string code) {
	this->errorMsg = "";
	this->tokens.clear();

	this->code = code;
	this->lookIndex = 0;
	this->look = code[0];
	this->codeEndIndex = code.length();
	this->currentCodeLine = 1;
	this->currentLineText = this->look;

	while (this->isNotEnd()) {
		this->next();
	}

	this->addToken(InterpreterTokenType::NewLine, this->currentLineText);
}

void Tokenizer::throwError(string message) {
	this->errorMsg = "Tokenizer error on line "+ to_string(this->currentCodeLine) + ": " + message;
	throw 'T';
}

void Tokenizer::addToken(InterpreterTokenType type, string value) {
	TokenObj newToken(type, value, this->currentCodeLine);
	this->tokens.push_back(newToken);
}

void Tokenizer::addToken(InterpreterTokenType type, double value) {
	TokenObj newToken(type, value, this->currentCodeLine);
	this->tokens.push_back(newToken);
}

void Tokenizer::addToken(InterpreterTokenType type) {
	TokenObj newToken(type, 0, this->currentCodeLine);
	this->tokens.push_back(newToken);
}

bool Tokenizer::isNotEnd() {
	return this->lookIndex < this->codeEndIndex;
}

void Tokenizer::getChar() {
	if (this->isNotEnd()) {
		this->lookIndex++;
		this->look = this->code[this->lookIndex];

		if (this->look) this->currentLineText += this->look;
	}
}

void Tokenizer::skipWhite() {
	while (this->isNotEnd() && isSpace(this->look)) {
		if (this->look == '\n') {
			this->currentCodeLine++;
			this->addToken(InterpreterTokenType::NewLine, this->currentLineText);
			this->currentLineText = "";
		}
		this->getChar();
	}
}

void Tokenizer::doubleLiteral() {
	bool hasDec = false;
	bool notDone = true;
	string num;

	while (this->isNotEnd() && notDone) {
		notDone = false;
		if (isDigit(this->look)) {
			num += this->look;
			notDone = true;
		}
		if (this->look == '.' && !hasDec) {
			num += this->look;
			hasDec = true;
			notDone = true;
		}
		if (notDone) this->getChar();
	}

	if (num.length() == 1 && hasDec) {
		this->addToken(InterpreterTokenType::Dot);
	} else if (num.length() > 0) {
		this->addToken(InterpreterTokenType::DoubleLiteral, stod(num));
	} else {
		this->throwError("expected number but found '" + to_string(this->look) + "'");
	}
}

void Tokenizer::stringLiteral() {
	char stringTerminator = this->look;
	string str;

	this->getChar();
	
	while (this->isNotEnd() && this->look != stringTerminator) {
		str += this->look;
		this->getChar();
	}
	if (!this->isNotEnd() && this->look!=stringTerminator) {
		this->throwError("expected string terminator but found end of code");
	}
	this->getChar();
	this->addToken(InterpreterTokenType::StringLiteral, str);
}

void Tokenizer::ident() {
	string name;
	bool notDone = true;
	
	while (this->isNotEnd() && notDone) {
		notDone = false;
		if (isAlNum(this->look) || isDigit(this->look) || this->look == '_' || this->look == '.') {
			name += this->look;
			notDone = true;
			this->getChar();
		}
	}

	if (name.length() == 0) this->throwError("expected identifier, but got nothing");

	if (name == "if") {

	} else if (name == "while") {
		this->addToken(InterpreterTokenType::While);
	} else if (name == "for") {
		this->addToken(InterpreterTokenType::For);
	} else if (name == "loop") {
		this->addToken(InterpreterTokenType::Loop);
	} else if (name == "else") {
		this->addToken(InterpreterTokenType::Else);
	} else if (name == "break") {
		this->addToken(InterpreterTokenType::Break);
	} else if (name == "return") {
		this->addToken(InterpreterTokenType::Return);
	} else if (name == "exit") {
		this->addToken(InterpreterTokenType::Exit);
	} else if (name == "floor") {
		this->addToken(InterpreterTokenType::Floor);
	} else if (name == "ceil") {
		this->addToken(InterpreterTokenType::Ceil);
	} else if (name == "min") {
		this->addToken(InterpreterTokenType::Min);
	} else if (name == "max") {
		this->addToken(InterpreterTokenType::Max);
	} else if (name == "clamp") {
		this->addToken(InterpreterTokenType::Clamp);
	} else if (name == "abs") {
		this->addToken(InterpreterTokenType::Abs);
	} else if (name == "lcase") {
		this->addToken(InterpreterTokenType::LCase);
	} else if (name == "ucase") {
		this->addToken(InterpreterTokenType::UCase);
	} else if (name == "trim") {
		this->addToken(InterpreterTokenType::Trim);
	} else if (name == "len") {
		this->addToken(InterpreterTokenType::Len);
	} else if (name == "substr") {
		this->addToken(InterpreterTokenType::SubStr);
	} else if (name == "void") {
		this->addToken(InterpreterTokenType::Void);
	} else if (name == "double") {
		this->addToken(InterpreterTokenType::Double);
	} else if (name == "string") {
		this->addToken(InterpreterTokenType::String);
	} else if (name == "bool") {
		this->addToken(InterpreterTokenType::Bool);
	} else if (name == "true") {
		this->addToken(InterpreterTokenType::True);
	} else if (name == "false") {
		this->addToken(InterpreterTokenType::False);
	} else if (name == "null") {
		this->addToken(InterpreterTokenType::Null);
	} else {
		this->addToken(InterpreterTokenType::Ident, name);
	}
}

void Tokenizer::next() {
	this->skipWhite();
	if (this->isNotEnd()) {
		if (isDigit(this->look) || this->look == '.') {
			this->doubleLiteral();
		} else if (isAlpha(this->look) || this->look == '_') {
			this->ident();
		} else if (this->look == '"' || this->look == '\'') {
			this->stringLiteral();
		} else {

			char symbol = this->look;
			this->getChar();
			switch (symbol) {
			case ';':
				this->addToken(InterpreterTokenType::LineDelim);
				break;
			case ',':
				this->addToken(InterpreterTokenType::Comma);
				break;

			case '?':
				this->addToken(InterpreterTokenType::Question);
				break;
			case ':':
				this->addToken(InterpreterTokenType::Colon);
				break;

			case '{':
				this->addToken(InterpreterTokenType::LeftCurly);
				break;
			case '}':
				this->addToken(InterpreterTokenType::RightCurly);
				break;

			case '[':
				this->addToken(InterpreterTokenType::LeftSquare);
				break;
			case ']':
				this->addToken(InterpreterTokenType::RightSquare);
				break;

			case '(':
				this->addToken(InterpreterTokenType::LeftParen);
				break;
			case ')':
				this->addToken(InterpreterTokenType::RightParen);
				break;

			case '^':
				this->addToken(InterpreterTokenType::Exponent);
				break;
			case '%':
				this->addToken(InterpreterTokenType::Mod);
				break;
			case '+':
				this->addToken(InterpreterTokenType::Plus);
				break;
			case '-':
				this->addToken(InterpreterTokenType::Minus);
				break;
			case '*':
				this->addToken(InterpreterTokenType::Multiply);
				break;
			case '/':
				if (this->isNotEnd() && this->look == '/') {
					this->getChar();
					while (this->isNotEnd() && this->look != '\n') {
						this->getChar();
					}
					break;
				}
				this->addToken(InterpreterTokenType::Divide);
				break;

			case '|':
				if (this->isNotEnd() && this->look == '|') {
					this->getChar();
					this->addToken(InterpreterTokenType::Or);
					break;
				}
				this->throwError("incomplete OR operator found, OR operators must be of boolean type '||'");
				break;

			case '&':
				if (this->isNotEnd() && this->look == '&') {
					this->getChar();
					this->addToken(InterpreterTokenType::And);
					break;
				}
				this->throwError("incomplete AND operator found, AND operators must be of boolean type '&&'");
				break;

			case '!':
				if (this->isNotEnd() && this->look == '=') {
					this->getChar();
					this->addToken(InterpreterTokenType::NotEquals);
					break;
				}
				this->addToken(InterpreterTokenType::Not);
				break;

			case '=':
				if (this->isNotEnd() && this->look == '=') {
					this->getChar();
					this->addToken(InterpreterTokenType::Equals);
					break;
				}
				this->addToken(InterpreterTokenType::Assignment);
				break;

			case '>':
				if (this->isNotEnd() && this->look == '=') {
					this->getChar();
					this->addToken(InterpreterTokenType::GreaterEquals);
					break;
				}
				this->addToken(InterpreterTokenType::Greater);
				break;
			case '<':
				if (this->isNotEnd() && this->look == '=') {
					this->getChar();
					this->addToken(InterpreterTokenType::LesserEquals);
					break;
				}
				this->addToken(InterpreterTokenType::Lesser);
				break;

			default:
				this->throwError("Unexpected symbol found, " + symbol);
			}
		}
	}
}