#include "Calculation.h"
#include <sstream>
#include <math.h>
#include <algorithm>

Calculation::Calculation(string expr, mode_t m)
{
	expression = expr;
	res = 0;
	mode = m;
	vector<word_t> word_seq = DivWords(expr);
	if (error_info == "")
		Operate(word_seq);
	if (error_info != "") // errors occur
	{
		output = error_info;
	}
	else // no error
	{
		long double integ = floor((long double)res * 1e6);
		if (res * 1e6 - integ>0.5)
			integ++;
		else if (res * 1e6 - integ<-0.5)
			integ--;
		res = integ / 1e6;
		stringstream ss;
		ss << res;
		string res_str;
		ss >> res_str;
		output = "\033[92mResult \033[0m" + res_str;
	}
}

Calculation::~Calculation()
{
}

string Calculation::Res()
{
	return output;
}

void Calculation::SetMode(mode_t m)
{
	mode = m;
}

vector<word_t> Calculation::DivWords(string expr)
{
	vector<word_t> ret;
	unsigned int pointer = 0;
	while (pointer < expr.size())
	{
		if (expr[pointer] == ' ') // ignore white space
		{
			pointer++;
		}
		else
		{
			unsigned int len = 1;
			bool innumber = false;
			bool meetdot = false;
			if (expr[pointer] >= '0' && expr[pointer] <= '9')
				innumber = true;
			while (pointer + len <= expr.size())
			{
				if (innumber == true) // it's a number
				{
					if (pointer + len < expr.size() && expr[pointer + len] == '.')
					{
						if (meetdot == false)
							meetdot = true;
						else
						{
							GenErrorInfo(pointer, len, "Too many dots in a real number.");
						}
					}
					if (pointer + len == expr.size()
						|| (expr[pointer + len] != '.' && !(expr[pointer + len] >= '0' && expr[pointer + len] <= '9')))
					{
						string number_str = expr.substr(pointer, len);
						double number;
						stringstream ss;
						ss << number_str;
						ss >> number;
						word_t word;
						word.type = OPERAND;
						word.val.oprd = number;
						word.offset = pointer;
						word.length = len;
						ret.push_back(word);
						break;
					}
				}
				else // it's a symbol
				{
					bool is_valid_symbol = false;
					for (unsigned int i = 0; i < symboltable.size(); i++)
					{
						string tempsymbol = expr.substr(pointer, len);
						transform(tempsymbol.begin(), tempsymbol.end(), tempsymbol.begin(), ::tolower);
						if (tempsymbol == symboltable[i].symbol)
						{
							ret.push_back(symboltable[i].value);
							(ret.end() - 1)->offset = pointer;
							(ret.end() - 1)->length = len;
							is_valid_symbol = true;
							break;
						}
					}
					if (is_valid_symbol == true)
						break;
				}
				len++;
			}
			if (pointer + len > expr.size()) // meet invalid expression
				break;
			pointer += len;
		}
	}
	//turn sub to neg if necessary
	for (unsigned int i = 0; i < ret.size(); i++)
	{
		if (ret[i].type == OPERATOR && ret[i].val.oprt == SUB)
		{
			if (!(i > 0 &&
				(ret[i - 1].type == OPERAND || ret[i - 1].val.oprt == RPAREN)))
			{
				ret[i].val.oprt = NEG;
			}
		}
	}
	if (pointer != expr.size()) // meet a syntax error
	{
		GenErrorInfo(pointer, 1, "Unreadable expression.");
	}
	return ret;
}

void Calculation::Operate(vector<word_t> word_seq)
{
	//error detection
	if (!(word_seq.begin()->type == OPERAND
		|| word_seq.begin()->val.oprt == SIN || word_seq.begin()->val.oprt == COS || word_seq.begin()->val.oprt == TAN
		|| word_seq.begin()->val.oprt == NEG
		|| word_seq.begin()->val.oprt == LPAREN)) // beginning
	{
		GenErrorInfo(word_seq.begin()->offset, word_seq.begin()->length, "Invalid expression beginning.");
		return;
	}
	else if (!((word_seq.end() - 1)->type == OPERAND
		|| (word_seq.end() - 1)->val.oprt == RPAREN)) // ending
	{
		GenErrorInfo((word_seq.end() - 1)->offset, (word_seq.end() - 1)->length, "Invalid expression ending.");
		return;
	}
	else // neighbor relationship
	{
		for (auto i = word_seq.begin(); i < word_seq.end() - 1; i++)
		{
			if (i->val.oprt == ADD || i->val.oprt == SUB || i->val.oprt == MUL || i->val.oprt == DIV || i->val.oprt == NEG)
			{
				if (!((i + 1)->type == OPERAND
					|| (i + 1)->val.oprt == SIN || (i + 1)->val.oprt == COS || (i + 1)->val.oprt == TAN
					|| (i + 1)->val.oprt == LPAREN
					|| (i + 1)->val.oprt == NEG))
				{
					GenErrorInfo((i + 1)->offset, (i + 1)->length, "Invalid keyword after arithmetic operators.");
					return;
				}
			}
			else if (i->val.oprt == SIN || i->val.oprt == COS || i->val.oprt == TAN)
			{
				if (!((i + 1)->type == OPERAND
					|| (i + 1)->val.oprt == SIN || (i + 1)->val.oprt == COS || (i + 1)->val.oprt == TAN
					|| (i + 1)->val.oprt == LPAREN
					|| (i + 1)->val.oprt == NEG))
				{
					GenErrorInfo((i + 1)->offset, (i + 1)->length, "Invalid keyword after trigonometric operators.");
					return;
				}
			}
			else if (i->type == OPERAND)
			{
				if (!((i + 1)->val.oprt == RPAREN
					|| (i + 1)->val.oprt == ADD || (i + 1)->val.oprt == SUB || (i + 1)->val.oprt == MUL || (i + 1)->val.oprt == DIV))
				{
					GenErrorInfo((i + 1)->offset, (i + 1)->length, "Invalid keyword after real number.");
					return;
				}
			}
			else if (i->val.oprt == LPAREN)
			{
				if (!((i + 1)->type == OPERAND
					|| (i + 1)->val.oprt == SIN || (i + 1)->val.oprt == COS || (i + 1)->val.oprt == TAN
					|| (i + 1)->val.oprt == NEG
					|| (i + 1)->val.oprt == LPAREN))
				{
					GenErrorInfo((i + 1)->offset, (i + 1)->length, "Invalid keyword after left parentheses.");
					return;
				}
			}
			else if (i->val.oprt == RPAREN)
			{
				if (!((i + 1)->val.oprt == RPAREN
					|| (i + 1)->val.oprt == ADD || (i + 1)->val.oprt == SUB || (i + 1)->val.oprt == MUL || (i + 1)->val.oprt == DIV))
				{
					GenErrorInfo((i + 1)->offset, (i + 1)->length, "Invalid keyword after right parentheses.");
					return;
				}
			}
		}
	}
	vector<word_t> paren_stack;
	for (auto i = word_seq.begin(); i < word_seq.end(); i++) // parenthesis matching
	{
		if (i->type == OPERATOR && i->val.oprt == LPAREN)
		{
			paren_stack.push_back(*i);
		}
		else if (i->type == OPERATOR && i->val.oprt == RPAREN)
		{
			if (!paren_stack.empty())
				paren_stack.erase(paren_stack.end() - 1);
			else
			{
				GenErrorInfo(i->offset, i->length, "Unpaired right parentheses.");
				return;
			}
		}
	}
	if (!paren_stack.empty())
	{
		GenErrorInfo(paren_stack.begin()->offset, paren_stack.begin()->length, "Unpaired left parentheses.");
		return;
	}

	//pre-operating
	word_t boundary;
	boundary.type = OPERATOR;
	boundary.val.oprt = BOUNDARY;
	word_seq.insert(word_seq.begin(), boundary);
	word_seq.push_back(boundary);

	//operaing
	operator_stack.push_back(word_seq[0]);
	for (unsigned int i = 1; i < word_seq.size(); i++)
	{
		if (word_seq[i].type == OPERATOR)
		{
			if (PriorityCompare(word_seq[i].val.oprt, (operator_stack.end() - 1)->val.oprt) == LOW)
			{
				i--;
				// unary or binary operating
				double a, b;
				a = *(operand_stack.end() - 1);
				if (operand_stack.size() > 1)
					b = *(operand_stack.end() - 2);
				double a_copy = a; // get the approximate value of a
				long double integ = floor((long double)a_copy * 1e6);
				if (a_copy * 1e6 - integ > 0.5)
					integ++;
				else if (a_copy * 1e6 - integ < -0.5)
					integ--;
				a_copy = integ / 1e6;
				switch ((operator_stack.end() - 1)->val.oprt)
				{
				case ADD:operand_stack.erase(operand_stack.end() - 2, operand_stack.end()); operand_stack.push_back(b + a); break;
				case SUB:operand_stack.erase(operand_stack.end() - 2, operand_stack.end()); operand_stack.push_back(b - a); break;
				case MUL:operand_stack.erase(operand_stack.end() - 2, operand_stack.end()); operand_stack.push_back(b * a); break;
				case DIV:
					if (a_copy == 0)
					{
						GenErrorInfo((operator_stack.end()-1)->offset, (operator_stack.end() - 1)->length, "Non-zero divisor.");
						return;
					}
					operand_stack.erase(operand_stack.end() - 2, operand_stack.end()); operand_stack.push_back(b / a); break;
				case NEG:operand_stack.erase(operand_stack.end() - 1); operand_stack.push_back(-a); break;
				case SIN:operand_stack.erase(operand_stack.end() - 1);
					if (mode == RAD) operand_stack.push_back(sin(a));
					else operand_stack.push_back(sin(a * 3.1415926 / 180));
					break;
				case COS:operand_stack.erase(operand_stack.end() - 1);
					if (mode == RAD) operand_stack.push_back(cos(a));
					else operand_stack.push_back(cos(a * 3.1415926 / 180));
					break;
				case TAN:operand_stack.erase(operand_stack.end() - 1);
					if (mode == RAD) operand_stack.push_back(tan(a));
					else operand_stack.push_back(tan(a * 3.1415926 / 180));
					break;
				}
				operator_stack.erase(operator_stack.end() - 1);
			}
			else if (PriorityCompare(word_seq[i].val.oprt, (operator_stack.end() - 1)->val.oprt) == SAME)
			{
				operator_stack.erase(operator_stack.end() - 1);
			}
			else
			{
				operator_stack.push_back(word_seq[i]);
			}
		}
		else
		{
			operand_stack.push_back(word_seq[i].val.oprd);
		}
	}
	if (!operand_stack.empty())
		res = *operand_stack.begin();
	else
		res = 0;
}

void Calculation::GenErrorInfo(unsigned int pos, unsigned int len, string info)
{
	string colored_expr = expression.substr(0, pos)
		+ "\033[91m" + expression.substr(pos, len) + "\033[0m"
		+ expression.substr(pos + len, expression.size() - pos - len)
		+ "\n";
	string point_str;
	for (unsigned int i = 0; i < pos; i++)
		point_str += " ";
	point_str += "\033[91m^";
	for (unsigned int i = 1; i < len; i++)
		point_str += "~";
	point_str += "\n";
	error_info = colored_expr + point_str + "\033[91m[error] \033[0m" + info;
}

vector<symbol_value_item> symboltable(12);

void InitSymboltable()
{
	symboltable[0].symbol = "sin";
	symboltable[0].value.type = OPERATOR;
	symboltable[0].value.val.oprt = SIN;
	symboltable[1].symbol = "cos";
	symboltable[1].value.type = OPERATOR;
	symboltable[1].value.val.oprt = COS;
	symboltable[2].symbol = "tan";
	symboltable[2].value.type = OPERATOR;
	symboltable[2].value.val.oprt = TAN;
	symboltable[3].symbol = "+";
	symboltable[3].value.type = OPERATOR;
	symboltable[3].value.val.oprt = ADD;
	symboltable[4].symbol = "-";
	symboltable[4].value.type = OPERATOR;
	symboltable[4].value.val.oprt = SUB;
	symboltable[5].symbol = "*";
	symboltable[5].value.type = OPERATOR;
	symboltable[5].value.val.oprt = MUL;
	symboltable[6].symbol = "/";
	symboltable[6].value.type = OPERATOR;
	symboltable[6].value.val.oprt = DIV;
	symboltable[7].symbol = "(";
	symboltable[7].value.type = OPERATOR;
	symboltable[7].value.val.oprt = LPAREN;
	symboltable[8].symbol = ")";
	symboltable[8].value.type = OPERATOR;
	symboltable[8].value.val.oprt = RPAREN;
	symboltable[9].symbol = "pi";
	symboltable[9].value.type = OPERAND;
	symboltable[9].value.val.oprd = 3.1415926;
	symboltable[10].symbol = "PI";
	symboltable[10].value.type = OPERAND;
	symboltable[10].value.val.oprd = 3.1415926;
	symboltable[11].symbol = "Pi";
	symboltable[11].value.type = OPERAND;
	symboltable[11].value.val.oprd = 3.1415926;
}

relpriority_t PriorityCompare(operator_t a, operator_t b)
{
	if (b == LPAREN && a == RPAREN)
		return SAME;
	else if (b == BOUNDARY && a == BOUNDARY)
		return SAME;
	else if (a == LPAREN || b == LPAREN)
		return HIGH;
	else if (a == RPAREN || b == RPAREN)
		return LOW;
	else if ((a == SIN || a == COS || a == TAN)
		&& (b == SIN || b == COS || b == TAN))
	{
		return HIGH;
	}
	else if ((a == SIN || a == COS || a == TAN)
		&& (b == NEG))
	{
		return HIGH;
	}
	else
	{
		if (a - b > 0)
		{
			return HIGH;
		}
		else
		{
			return LOW;
		}
	}
}
