# PROJECT 3 - TRIGONOMETRY CALCULATOR

> Auther: stupidpanther
> E-mail: stupidpanther@163.com

### Requirement Analysis

As a trigonometry calculator, it receive a user-input expression and print the result if valid. Generally speaking, it has following functions.
+ reading expression from user input
+ judging the validity of the expression
+ analyzing expression syntax and dividing it into several unary or binary expression
+ calculating
+ print the result

To improve user exprience, it ought to have following functions also.
+ indicating the errors of expression syntax visually if existing
+ recording history

### Module Design

Here is a module design of the project.
![Module Design](./assets/TrigonometryCalculator-ModuleDesign.png)

### Data Structure

+ `class Interaction`
``` cpp
class Interaction
{
private:
	string expression;
	vector<string> history;
	vector<string> history_edit;
	unsigned int history_len;
	history_mode_t history_mode;
	mode_t mode;

public:
	Interaction();
	~Interaction();
	int Exec();

private:
	void ReadExpression();
	void Output(string output);
	void PrintPrompt();
};
```

+ `class Calculation`
``` cpp
class Calculation
{
private:
	string expression;
	vector<word_t> operator_stack;
	vector<double> operand_stack;
	string error_info;
	string error_crc;
	double res;
	string output;
	mode_t mode;

public:
	Calculation(string expr, mode_t m);
	~Calculation();
	string Res();
	void SetMode(mode_t m);

private:
	vector<word_t> DivWords(string expr);
	void Operate(vector<word_t> word_seq);
	void GenErrorInfo(unsigned int pos, unsigned int len, string info);
	void GenErrorCrc(string crc);
};
```

### Core Algorithm

In general, there are several queations for us to answer: How to transfer the user-input expression to program understandable expression? How to calculate the program understandable expression? How to locate where user made syntax mistakes, if existed? Could we help the user to correct syntax mistakes? How to record history?
This part focuses on the contents mentioned above.

+ **From user-input expression to program understandable expression**
    This process can be divided into following sub-processes:
    + word division
    + generating pseudo postfix expression using stack structure
+ **Calculating the program understandable expression**
    It should be concentrated here that differences exists between unary operator and binary operator.
    We calculate the program understandable expression by operator priority. Also, we use library functions to implement low-level operations.
+ **Syntax mistakes location**
    Record the pointer location when reading user-input expression. It indicates where the mistake is.
+ **Syntax mistakes auto correction**
    This process will be discussed later.
+ **History**
    Add a new entry to history when user press enter key once.

### Special Thanks
+ Analyzing expression using stack:
[https://leetcode-cn.com/problems/basic-calculator/solution/qiao-yong-yun-suan-fu-you-xian-ji-biao-he-zhan-shi/](https://leetcode-cn.com/problems/basic-calculator/solution/qiao-yong-yun-suan-fu-you-xian-ji-biao-he-zhan-shi/)
+ Infix expression and postfix expression:
[https://zhuanlan.zhihu.com/p/123315675](https://zhuanlan.zhihu.com/p/123315675)
+ Color control in terminal
[https://blog.csdn.net/RadiantJeral/article/details/105456642](https://blog.csdn.net/RadiantJeral/article/details/105456642)
+ Implement a customized shell using C language
[https://blog.csdn.net/aiwangtingyun/article/details/79636990](https://blog.csdn.net/aiwangtingyun/article/details/79636990)
+ VT 100 control code
[https://blog.csdn.net/liufei191010/article/details/81015445](https://blog.csdn.net/liufei191010/article/details/81015445)
