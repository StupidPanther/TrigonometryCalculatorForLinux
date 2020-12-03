#pragma once
#include "ref.h"
#include "Calculation.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum history_mode_t
{
	SIMPLE,
	FULL
};

class Interaction
{
private:
	string expression;
	vector<string> history;
	vector<string> history_edit;
	unsigned int history_len;
	history_mode_t history_mode;
	calmode_t mode;

public:
	Interaction();
	~Interaction();
	int Exec();

private:
	void ReadExpression();
	void Output(string output);
	void PrintPrompt();
};

