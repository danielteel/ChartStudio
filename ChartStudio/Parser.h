#pragma once
#include <vector>
#include <string>
#include "Tokenizer.h"
#include "OpObj.h"
#include "Program.h"
#include "UnlinkedObj.h"
#include "ExternalDef.h"
using namespace std;


class ScopeObj {
	public:
		string name;
		IdentityType type;
		size_t branch;
		vector<IdentityType> params;
		IdentityType returnType;
		size_t scope;
		size_t index;

		ScopeObj(string name, IdentityType type, size_t branch, const vector<IdentityType>& params, IdentityType returnType, size_t scope, size_t index) {
			this->name = name;
			this->type = type;
			this->branch = branch;
			this->params = params;
			this->returnType = returnType;
			this->scope = scope;
			this->index = index;
		}

		ScopeObj(string name, IdentityType type, size_t scope, size_t index) {
			this->name = name;
			this->type = type;
			this->scope = scope;
			this->index = index;
		}

		ScopeObj(const ScopeObj& a) {
			this->name = a.name;
			this->type = a.type;
			this->branch = a.branch;
			this->params = a.params;
			this->returnType = a.returnType;
			this->scope = a.scope;
			this->index = a.index;
		}


};

class Parser {
public:
	Parser(vector<TokenObj>& tokens);
	~Parser();
	size_t newBranch();
	void throwError(string message);
	void typeMismatch(IdentityType expectedType, IdentityType foundType);
	void matchType(IdentityType whatToTest, IdentityType typeItShouldBe, bool strict);
	bool typesDontMatch(IdentityType a, IdentityType b, bool strict);
	void assertType(IdentityType a, IdentityType b, bool strict);
	void match(InterpreterTokenType type);
	bool isNotEnd();
	void getToken();
	void firstToken();
	void parse(vector<ExternalDef> externals, optional<IdentityType> exitType = nullopt);
	void pushAllocScope();
	void popAllocScope();
	void pushScope();
	void popScope();

	ScopeObj addToCurrentScope(string name, IdentityType type, size_t branch, const vector<IdentityType>, IdentityType returnType);
	ScopeObj * getIdentity(string name, bool onlyInCurrentScope);
	ScopeObj addVar(string name, IdentityType type);
	ScopeObj addFunction(string name, IdentityType returnType, size_t branch, vector<IdentityType> params);

	bool isPowerOp();
	bool isTermOp();
	bool isAddOp();
	bool isCompareOp();
	bool isOrOp();
	bool isAndOp();
	bool isTernaryOp();

	IdentityType doFuncCall(string funcName);
	IdentityType doIdent();
	IdentityType doFactor();
	IdentityType doExponentiation();
	IdentityType doTerm();
	IdentityType doAdd();
	IdentityType doCompare();
	IdentityType doAnd();
	IdentityType doOr();
	IdentityType doExpression();

	void doIf(optional<size_t> breakToBranch, optional<size_t> returnToBranch, optional<IdentityType> returnType);
	void doWhile(optional<size_t> returnToBranch, optional<IdentityType> returnType);
	void doLoop(optional<size_t> returnToBranch, optional<IdentityType> returnType);
	void doFor(optional<size_t> returnToBranch, optional<IdentityType> returnType);
	void doBreak(optional<size_t> breakToBranch);
	void doExit();
	void doAssignOrDeclare(bool cantBeFunction);
	void doDeclare(bool cantBeFunction);
	void doFunction(string name, IdentityType type);
	void doReturn(optional<size_t> returnToBranch, optional<IdentityType> returnType);
	void doAssignment(bool wantsDelim);
	void doIdentStatement();
	void doStatement(optional<size_t> breakToBranch, optional<size_t> returnToBranch, optional<IdentityType> returnType);

	void doBlock(optional<size_t> breakToBranch, optional<size_t> returnToBranch, bool ifNeedsCurlys, bool couldBeStatement, bool dontPushScope, optional<IdentityType> returnType);

	string errorMsg;

	optional<IdentityType> exitType = nullopt;

	vector<TokenObj> tokens;
	size_t tokenIndex = 0;
	TokenObj* token;
	size_t tokenEndIndex = 0;

	size_t branchCounter = 0;
	size_t scopeIndex = 0;
	size_t allocScopeIndex = 0;
	size_t maxScopeDepth = 0;
	size_t externalsScopeIndex = 0;


	vector<size_t> allocScope;
	vector<vector<ScopeObj>> scopes;

	Program program;
};
