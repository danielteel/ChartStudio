#pragma once
#include <string>
#include <vector>
#include <stack>
#include <optional>
using namespace std;

class ChartProject;
class CChartObject;
class CChartBase;

enum class TokenType {
	Constant,
	Ident,
	LeftParen,
	RightParen,
	Exponent,
	Plus,
	Minus,
	Divide,
	Multiply,
	Assignment,
	Equals,
	NotEquals,
	Lesser,
	Greater,
	LesserEquals,
	GreaterEquals,
	FuncMin,
	FuncMax,
	FuncAbs,
	FuncClamp,
	FuncBoolIsNil,
	While,
	If,
	LeftCurly,
	RightCurly,
	Return,
	Else,
	Break,
	LineDelim,
	Not,
	And,
	Or,
	Comma,
	Double,
	True,
	False,
	Nil,
	ChartCall,
	This,
	String,
	MsgBox
};

struct Token {
	enum TokenType type;

	Token(enum TokenType type) {
		this->type = type;
	}
};

struct TConstant : Token {
	double value;

	TConstant(double value) : Token(TokenType::Constant) {
		this->value = value;
	}
};

struct TIdent : Token {
	string name;

	TIdent(string name) : Token(TokenType::Ident) {
		this->name = name;
	}
};

struct TChartCall :Token {
	string name;

	TChartCall(string name) : Token(TokenType::ChartCall) {
		this->name = name;
	}
};

struct TString : Token {
	string str;
	TString(string str) : Token(TokenType::String) {
		this->str = str;
	}
};

struct Variable {
	string name;
	bool isConstant;
	optional<double> value;

	Variable(string name, bool isConstant, optional<double> value = nullopt) {
		this->name = name;
		this->isConstant = isConstant;
		this->value = value;
	}
};

struct ChartFunction {
	string name;
	CChartBase* chartBase;

	ChartFunction(string name, CChartBase* chartBase) {
		this->name = name;
		this->chartBase = chartBase;
	}
};

typedef int BranchID;

enum class ROp {
	push,
	pop,
	mov,
	load,
	add,
	sub,
	div,
	mul,
	neg,
	power,
	andop,
	orop,
	notop,
	clear,
	set,
	cmp,
	cmptoconstant,
	test,
	label,
	jmp,
	je,
	jne,
	ja,
	jae,
	jb,
	jbe,
	se,
	sne,
	sa,
	sae,
	sb,
	sbe,
	ret,
	isnil,
	calllinear,
	callclamp,
	callpoly,
	calltrend,
	setThis,
	getThis,
	msgbox
};

struct ROperation {
	int codeLine = 0;
	enum ROp type;
	BranchID thisLabelID=0;

	optional<double>* double1 = nullptr;
	optional<double>* double2 = nullptr;
	optional<double>* double3 = nullptr;

	CChartBase* chartBase = nullptr;

	string* string1 = nullptr;

	optional<double> constant=nullopt;

	BranchID toLabelID=0;
	vector<ROperation*>::iterator toLabel;

	ROperation(int codeLine, enum ROp type) {
		this->type = type;
		this->codeLine = codeLine;
	}


	static ROperation* newPush(int codeLine, optional<double>* double1) {
		ROperation* newROp = new ROperation(codeLine, ROp::push);
		newROp->double1 = double1;
		return newROp;
	}
	static ROperation* newPop(int codeLine, optional<double>* double1) {
		ROperation* newROp = new ROperation(codeLine, ROp::pop);
		newROp->double1 = double1;
		return newROp;
	}
	static ROperation* newMov(int codeLine, optional<double>* double1, optional<double>* double2) {
		ROperation* newROp = new ROperation(codeLine, ROp::mov);
		newROp->double1 = double1;
		newROp->double2 = double2;
		return newROp;
	}
	static ROperation* newLoad(int codeLine, optional<double>* double1, optional<double> constant) {
		ROperation* newROp = new ROperation(codeLine, ROp::load);
		newROp->double1 = double1;
		newROp->constant = constant;
		return newROp;
	}
	static ROperation* newAdd(int codeLine, optional<double>* double1, optional<double>* double2) {
		ROperation* newROp = new ROperation(codeLine, ROp::add);
		newROp->double1 = double1;
		newROp->double2 = double2;
		return newROp;
	}
	static ROperation* newSub(int codeLine, optional<double>* double1, optional<double>* double2) {
		ROperation* newROp = new ROperation(codeLine, ROp::sub);
		newROp->double1 = double1;
		newROp->double2 = double2;
		return newROp;
	}
	static ROperation* newDiv(int codeLine, optional<double>* double1, optional<double>* double2) {
		ROperation* newROp = new ROperation(codeLine, ROp::div);
		newROp->double1 = double1;
		newROp->double2 = double2;
		return newROp;
	}
	static ROperation* newMul(int codeLine, optional<double>* double1, optional<double>* double2) {
		ROperation* newROp = new ROperation(codeLine, ROp::mul);
		newROp->double1 = double1;
		newROp->double2 = double2;
		return newROp;
	}
	static ROperation* newNeg(int codeLine, optional<double>* double1) {
		ROperation* newROp = new ROperation(codeLine, ROp::neg);
		newROp->double1 = double1;
		return newROp;
	}
	static ROperation* newPower(int codeLine, optional<double>* double1, optional<double>* double2) {
		ROperation* newROp = new ROperation(codeLine, ROp::power);
		newROp->double1 = double1;
		newROp->double2 = double2;
		return newROp;
	}

	static ROperation* newAnd(int codeLine, optional<double>* double1, optional<double>* double2) {
		ROperation* newROp = new ROperation(codeLine, ROp::andop);
		newROp->double1 = double1;
		newROp->double2 = double2;
		return newROp;
	}
	static ROperation* newOr(int codeLine, optional<double>* double1, optional<double>* double2) {
		ROperation* newROp = new ROperation(codeLine, ROp::orop);
		newROp->double1 = double1;
		newROp->double2 = double2;
		return newROp;
	}
	static ROperation* newNotOp(int codeLine, optional<double>* double1) {
		ROperation* newROp = new ROperation(codeLine, ROp::notop);
		newROp->double1 = double1;
		return newROp;
	}
	static ROperation* newClear(int codeLine, optional<double>* double1) {
		ROperation* newROp = new ROperation(codeLine, ROp::clear);
		newROp->double1 = double1;
		return newROp;
	}
	static ROperation* newSet(int codeLine, optional<double>* double1) {
		ROperation* newROp = new ROperation(codeLine, ROp::set);
		newROp->double1 = double1;
		return newROp;
	}
	static ROperation* newTest(int codeLine, optional<double>* double1) {
		ROperation* newROp = new ROperation(codeLine, ROp::test);
		newROp->double1 = double1;
		return newROp;
	}
	static ROperation* newCmp(int codeLine, optional<double>* double1, optional<double>* double2) {
		ROperation* newROp = new ROperation(codeLine, ROp::cmp);
		newROp->double1 = double1;
		newROp->double2 = double2;
		return newROp;
	}
	static ROperation* newCmpToConstant(int codeLine, optional<double>* double1, optional<double> constant) {
		ROperation* newROp = new ROperation(codeLine, ROp::cmptoconstant);
		newROp->double1 = double1;
		newROp->constant = constant;
		return newROp;
	}
	static ROperation* newLabel(int codeLine, BranchID branchID) {
		ROperation* newROp = new ROperation(codeLine, ROp::label);
		newROp->thisLabelID = branchID;
		return newROp;
	}
	static ROperation* newJmp(int codeLine, BranchID branchID) {
		ROperation* newROp = new ROperation(codeLine, ROp::jmp);
		newROp->toLabelID = branchID;
		return newROp;
	}
	static ROperation* newJE(int codeLine, BranchID branchID) {
		ROperation* newROp = new ROperation(codeLine, ROp::je);
		newROp->toLabelID = branchID;
		return newROp;
	}
	static ROperation* newJNE(int codeLine, BranchID branchID) {
		ROperation* newROp = new ROperation(codeLine, ROp::jne);
		newROp->toLabelID = branchID;
		return newROp;
	}
	static ROperation* newJA(int codeLine, BranchID branchID) {
		ROperation* newROp = new ROperation(codeLine, ROp::ja);
		newROp->toLabelID = branchID;
		return newROp;
	}
	static ROperation* newJAE(int codeLine, BranchID branchID) {
		ROperation* newROp = new ROperation(codeLine, ROp::jae);
		newROp->toLabelID = branchID;
		return newROp;
	}
	static ROperation* newJB(int codeLine, BranchID branchID) {
		ROperation* newROp = new ROperation(codeLine, ROp::jb);
		newROp->toLabelID = branchID;
		return newROp;
	}
	static ROperation* newJBE(int codeLine, BranchID branchID) {
		ROperation* newROp = new ROperation(codeLine, ROp::jbe);
		newROp->toLabelID = branchID;
		return newROp;
	}
	static ROperation* newSE(int codeLine, optional<double>* double1) {
		ROperation* newROp = new ROperation(codeLine, ROp::se);
		newROp->double1 = double1;
		return newROp;
	}
	static ROperation* newSNE(int codeLine, optional<double>* double1) {
		ROperation* newROp = new ROperation(codeLine, ROp::sne);
		newROp->double1 = double1;
		return newROp;
	}
	static ROperation* newSA(int codeLine, optional<double>* double1) {
		ROperation* newROp = new ROperation(codeLine, ROp::sa);
		newROp->double1 = double1;
		return newROp;
	}
	static ROperation* newSAE(int codeLine, optional<double>* double1) {
		ROperation* newROp = new ROperation(codeLine, ROp::sae);
		newROp->double1 = double1;
		return newROp;
	}	
	static ROperation* newSB(int codeLine, optional<double>* double1) {
		ROperation* newROp = new ROperation(codeLine, ROp::sb);
		newROp->double1 = double1;
		return newROp;
	}
	static ROperation* newSBE(int codeLine, optional<double>* double1) {
		ROperation* newROp = new ROperation(codeLine, ROp::sbe);
		newROp->double1 = double1;
		return newROp;
	}
	static ROperation* newRet(int codeLine) {
		return new ROperation(codeLine, ROp::ret);
	}
	static ROperation* newIsNil(int codeLine, optional<double>* double1, optional<double>* double2) {
		ROperation* newROp = new ROperation(codeLine, ROp::isnil);
		newROp->double1 = double1;
		newROp->double2 = double2;
		return newROp;
	}
	static ROperation* newLinearChartCall(int codeLine, CChartBase* chart, optional<double>* double1, optional<double>* double2) {
		ROperation* newROp = new ROperation(codeLine, ROp::calllinear);
		newROp->double1 = double1;
		newROp->double2 = double2;
		newROp->chartBase = chart;
		return newROp;
	}
	static ROperation* newClampChartCall(int codeLine, CChartBase* chart, optional<double>* double1, optional<double>* double2) {
		ROperation* newROp = new ROperation(codeLine, ROp::callclamp);
		newROp->double1 = double1;
		newROp->double2 = double2;
		newROp->chartBase = chart;
		return newROp;
	}
	static ROperation* newPolyChartCall(int codeLine, CChartBase* chart, optional<double>* double1, optional<double>* double2) {
		ROperation* newROp = new ROperation(codeLine, ROp::callpoly);
		newROp->double1 = double1;
		newROp->double2 = double2;
		newROp->chartBase = chart;
		return newROp;
	}
	static ROperation* newTrendChartCall(int codeLine, CChartBase* chart, optional<double>* double1, optional<double>* double2, optional<double>* double3) {
		ROperation* newROp = new ROperation(codeLine, ROp::calltrend);
		newROp->double1 = double1;
		newROp->double2 = double2;
		newROp->double3 = double3;
		newROp->chartBase = chart;
		return newROp;
	}
	static ROperation* newSetThis(int codeLine) {
		ROperation* newROp = new ROperation(codeLine, ROp::setThis);
		return newROp;
	}
	static ROperation* newGetThis(int codeLine) {
		ROperation* newROp = new ROperation(codeLine, ROp::getThis);
		return newROp;
	}
	static ROperation* newMsgBox(int codeLine, string* string1) {
		ROperation* newROp = new ROperation(codeLine, ROp::msgbox);
		newROp->string1 = string1;
		return newROp;
	}

};


enum class InterpreterError {
	CODE,
	LINK,
	EXECUTION
};

class Interpreter {
public:
	Interpreter();
	~Interpreter();

	string getErrorMessage();
	int getErrorLine();
	enum InterpreterError getPhaseOfError();
	string getCode();

	string tokenTypeToString(enum TokenType type);

	bool checkCompile(ChartProject* chartProject, CChartObject* thisChartObject, string code, int& errorLine, string& errorMessage);


	bool runCode(ChartProject* chartProject, CChartObject* thisChartObject, string theCode, optional<double>& returnedValue);

private:
	//runtime variables
	vector<ROperation*>::iterator instructionPointer;
	stack<optional<double>> runStack;
	vector<string*> stringPool;

	optional<double> eax = nullopt;
	optional<double> ebx = nullopt;
	optional<double> ecx = nullopt;

	bool flag_AGreater = false;
	bool flag_BGreater = false;
	bool flag_Equals = false;


	vector<Variable*> variables;
	vector<ChartFunction*> chartFunctions;
	vector<ROperation*> compiledProgram;

	Variable* getVariable(string name);
	bool addVariable(string name, bool isConstant, optional<double> value = nullopt);
	void updateVariables(ChartProject* chartProject, CChartObject* thisChartObject);
	void clearVariables();

	ChartFunction * getChartFunction(string name);
	void updateChartFunctions(ChartProject* chartProject);
	void clearChartFunctions();

	bool getLabelIterator(BranchID branch, vector<ROperation*>::iterator& returnedIterator);

	string code;

	string errorString;
	int errorCodeLine;
	enum InterpreterError errorWasDuring;

	void addOp(ROperation* rop);
	void clearOps();

	void clearStringPool();

	string* newPoolString(string str);

	bool compile(ChartProject* chartProject, CChartObject* thisChartObject);
	bool link();
	bool execute(ChartProject* chartProject, optional<double>& returnedValue);

	BranchID branchCounter = 0;
	string::iterator look;
	string::iterator codeEnd;
	Token* token = nullptr;

	bool next();
	bool match(enum TokenType type);
	BranchID newBranch();
	bool doAbs();
	bool doMin();
	bool doMax();
	bool doClamp();
	bool doChartCall();
	void setToken(Token* tok);
	void getChar();
	void skipWhite();
	bool isNotEnd();
	bool number();
	bool stringToken();
	bool ident();

	bool chartCall();
	
	bool isOrOp();
	bool isAndOp();
	bool isCompareOp();
	bool isPowerOp();
	bool isTermOp();
	bool isAddOp();

	bool doFactor();
	bool doPower();
	bool doTerm();
	bool doAdd();
	bool doCompare();
	bool doAnd();
	bool doExpression();
	
	bool doIf(optional<BranchID> breakToBranch);
	bool doWhile(optional<BranchID> breakToBranch);
	bool doBreak(optional<BranchID> breakToBranch);
	bool doReturn();
	bool doDouble();
	bool doAssignmentWithName(string name);
	bool doAssignment();
	bool doBlock(optional<BranchID> breakToBranch, bool ifWantsRightCurly);

};

