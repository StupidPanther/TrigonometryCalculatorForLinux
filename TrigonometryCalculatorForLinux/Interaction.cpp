#include "Interaction.h"
#include <conio.h>
#include <sstream>

Interaction::Interaction()
{
	mode = RAD;
	InitSymboltable();
	history_len = 5;
	history_mode = SIMPLE;
}

Interaction::~Interaction()
{
}

int Interaction::Exec()
{
	while (true)
	{
		// print prompt
		PrintPrompt();
		// input
		ReadExpression();
		// expression processing
		if (expression == "quit" || expression == "exit") // program ending
		{
			break;
		}
		else if (expression == "rad" || expression == "RAD")
		{
			mode = RAD;
		}
		else if (expression == "deg" || expression == "DEG")
		{
			mode = DEG;
		}
		else if (expression == "clear")
		{
			system("cls");
		}
		else if (expression.substr(0, 12) == "history size" && expression.size() > 13)
		{
			string len_str = expression.substr(13, expression.size() - 13);
			stringstream ss;
			ss << len_str;
			unsigned int len;
			ss >> len;
			if (len > 0 && len < 128)
				history_len = len;
			if (history.size() > history_len)
			{
				for (unsigned int i = history.size(); i > history_len; i--)
				{
					history.erase(history.begin());
					history_edit.erase(history_edit.begin());
				}
			}
		}
		else if (expression == "history mode full")
		{
			history_mode = FULL;
		}
		else if (expression == "history mode simple")
		{
			history_mode = SIMPLE;
		}
		else if (expression == "")
		{
			continue;
		}
		else // calculating
		{
			Calculation calc(expression, mode);
			Output(calc.Res());
			if (history_mode==SIMPLE && calc.Res().substr(0, 11) != "\033[92mResult")
			{
				history.erase(history.end()-1);
				history_edit.erase(history_edit.end()-1);
			}
		}
	}
	return 0;
}

void Interaction::ReadExpression()
{
	expression = "";
	history_edit.push_back(expression);
	unsigned int index_history = history_edit.size() - 1;
	unsigned int cursor_position = 0;
	cout << "\033[s"; // save cursor position

	char ch = 0;
	ch = _getch();
	while (ch != '\r')
	{
		if (ch == '\b') //backspace
		{
			if (cursor_position > 0)
			{
				history_edit[index_history] = history_edit[index_history].substr(0, cursor_position - 1) + history_edit[index_history].substr(cursor_position, history_edit[index_history].size() - cursor_position);
				cursor_position--;
			}
		}
		else if (ch == '\t') //tab
		{
			;
		}
		else if (ch == -32)
		{
			ch = _getch();
			if (ch == 83) //delete
			{
				if (cursor_position < history_edit[index_history].size())
				{
					history_edit[index_history] = history_edit[index_history].substr(0, cursor_position) + history_edit[index_history].substr(cursor_position + 1, history_edit[index_history].size() - cursor_position);
				}
			}
			if (ch == 72) //up
			{
				if (index_history != 0)
					index_history--;
				else
					index_history = history_edit.size() - 1;
				cursor_position = history_edit[index_history].size();
			}
			else if (ch == 80) //down
			{
				if (index_history != history_edit.size() - 1)
					index_history++;
				else
					index_history = 0;
				cursor_position = history_edit[index_history].size();
			}
			else if (ch == 75) //left
			{
				if (cursor_position > 0)
					cursor_position--;
			}
			else if (ch == 77) //right
			{
				if (cursor_position < history_edit[index_history].size())
					cursor_position++;
			}
		}
		else //other characters
		{
			stringstream ss;
			ss << ch;
			string newch;
			getline(ss, newch);
			history_edit[index_history] = history_edit[index_history].substr(0, cursor_position) + newch + history_edit[index_history].substr(cursor_position, history_edit[index_history].size() - cursor_position);
			cursor_position++;
		}

		// print expression
		cout << "\033[u\033[K" << history_edit[index_history];
		cout << "\033[u";
		if (cursor_position > 0)
			cout << "\033[" << cursor_position << "C";
		ch = _getch();
	}

	expression = history_edit[index_history];
	history.push_back(history_edit[index_history]);
	history_edit[index_history] = history[index_history];
	*(history_edit.end() - 1) = *(history.end() - 1);
	if (*(history_edit.end() - 1) == "") // filter null expression
	{
		history.erase(history.end() - 1);
		history_edit.erase(history_edit.end() - 1);
	}
	if (history.size() > history_len)
	{
		for (unsigned int i = history.size(); i > history_len; i--)
		{
			history.erase(history.begin());
			history_edit.erase(history_edit.begin());
		}
	}

	cout << endl;
}

void Interaction::Output(string output)
{
	cout << output << endl;
}

void Interaction::PrintPrompt()
{
	cout << "\033[97;104m Calculator \033[0m";
	if (mode == RAD)
		cout << "\033[97;101m RAD \033[0m";
	else if(mode==DEG)
		cout << "\033[97;105m DEG \033[0m";
	cout << "\033[97;104m > \033[0m";
	cout << "  ";
}
