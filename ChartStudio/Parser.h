#pragma once
#include <vector>
#include <string>
#include "Tokenizer.h"
#include "OpObj.h"

using namespace std;

enum class IdentityType {
	Null,
	Bool,
	Double,
	String,
	Function
};

class ExternalDef {
	public:
		string name;
		IdentityType type;
		OpObj* opObj;
		void* functionAddress;
		vector<IdentityType> params;
		IdentityType returnType;

	ExternalDef(string name, IdentityType type, OpObj* opObj){
		this->name = name;
		this->type = type;
		this->opObj = opObj;
		this->functionAddress = nullptr;
	}

	ExternalDef(string name, IdentityType returnType, vector<IdentityType> params, void* functionAddress) {
		this->name = name;
		this->type = IdentityType::Function;
		this->returnType = returnType;
		this->params = params;
		this->functionAddress = functionAddress;
		this->opObj = nullptr;
	}
	
	ExternalDef() {
	}
};

class ScopeObj {
	public:
		string name;
		IdentityType type;
		size_t branch;
		vector<IdentityType> params;
		IdentityType returnType;
		size_t scope;
		size_t index;

		ScopeObj(string name, IdentityType type, size_t branch, vector<IdentityType>& params, IdentityType returnType, size_t scope, size_t index) {
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

		ScopeObj() {
		}
};

class Parser {
public:
	Parser(vector<TokenObj>& tokens);
	size_t newBranch();
	void throwError(string message);
	void typeMismatch(IdentityType expectedType, IdentityType foundType);
	void matchType(IdentityType whatToTest, IdentityType typeItShouldBe, bool strict);
	bool typesDontMatch(IdentityType a, IdentityType b, bool strict);
	void match(InterpreterTokenType type);
	bool isNotEnd();
	void getToken();
	void firstToken();
	void parse(vector<ExternalDef> externals);
	~Parser();

	string errorMsg;

	vector<TokenObj> tokens;
	size_t tokenIndex = 0;
	TokenObj* token;
	size_t tokenEndIndex = 0;

	size_t branchCounter = 0;
	size_t scopeIndex = 0;
	size_t allocScopeIndex = 0;
	size_t maxScopeDepth = 0;

	vector<size_t> allocScope;
	vector<vector<ScopeObj>> scopes;
};