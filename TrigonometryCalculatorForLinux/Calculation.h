#pragma once
#include "ref.h"
#include <stack>
#include <vector>
#include <string>
using namespace std;

enum operator_t
{
	BOUNDARY,
	LPAREN,
	RPAREN,
	ADD,
	SUB,
	MUL,
	DIV,
	SIN,
	COS,
	TAN,
	NEG,
};

enum wordtype_t
{
	OPERATOR,
	OPERAND
};

class word_t
{
public:
	wordtype_t type;
	union
	{
		operator_t oprt;
		double oprd;
	}val;
	unsigned int offset;
	unsigned int length;
};

class symbol_value_item
{
public:
	string symbol;
	word_t value;
};

extern vector<symbol_value_item> symboltable;

void InitSymboltable();

enum relpriority_t
{
	LOW,
	SAME,
	HIGH
};

relpriority_t PriorityCompare(operator_t a, operator_t b);

class Calculation
{
private:
	string expression;
	vector<word_t> operator_stack;
	vector<double> operand_stack;
	string error_info;
	double res;
	string output;
	calmode_t mode;

public:
	Calculation(string expr, calmode_t m);
	~Calculation();
	string Res();
	void SetMode(calmode_t m);

private:
	vector<word_t> DivWords(string expr);
	void Operate(vector<word_t> word_seq);
	void GenErrorInfo(unsigned int pos, unsigned int len, string info);
};

