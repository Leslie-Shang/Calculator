#include<iostream>
#include<stack>
#include<cmath>

using namespace std;

stack<double> num;//数字堆栈
stack<char> op;//运算符堆栈

int nagNuFlag = 1;
/*
负数标记，0代表之后读到的‘-’不做负数标记，
1代表之后读到的‘-’做负数标记，
*/
int squareFlag = 0;
/*
乘方操作，0代表不进行乘方，
1代表读取到乘方标记，等待幂的读入，
2代表幂的读入完成，即将进行乘方操作*/

double getInfo(string exp);//从字符串中获取数字及运算符信息
int getNum(string exp);//从字符串中获取数字信息
int getOp(char exp, int opType);//从字符串中获取运算符信息
void getRes(char op);//计算 num1 op num2 的值，并存入num栈顶
int getOpType(char op);//获取运算符优先级，2最高，0最低

double getInfo(string exp) {
	int length = 0;
	int opType = 0;
	
	int tmp;
	while ((tmp = exp.find("qrt")) > -1) {
		exp.erase(tmp, 3);
	}
	length = exp.size();

	for (int i = 0; i < length;) {//i用来标记当前读取位置
		int j = (exp[i] <= '9' && exp[i] >= '0') ? 1 : 0;//判断读到的是否为数字，true为1，false为0
		if (j) {			
			i += getNum(&exp[i]);//获取完整数字，并将读取标记移动到数字之后
			if (nagNuFlag == 1)nagNuFlag = 0;//清除负数标记		
		}
		else {
			//跳过乘方标记
			if (exp[i] == '^') {
				i++;
				squareFlag = 1;
				continue;
			}

			opType = getOp(exp[i], opType);//获取当前运算符堆栈中的最高优先级，并在需要时对运算符进行相应的操作
			i++;//读取位置移动到运算符之后
		}
		//数字被完整读入后，对其进行乘方操作
		if ((squareFlag == 1) && (exp[i] != '.')) {
			int Num = num.top();
			num.pop();
			num.top() = pow(num.top(), Num);
			squareFlag = 0;
		}
		
	}
	//运算式读取完成后，若运算符堆栈非空，则将其内的所有内容栈，并对其运算符进行相应的操作
	for (; op.size() > 0;) {
		getRes(op.top());
		op.pop();
	}

	return num.top();
}
//获取完整数字，并返回读取操作次数
int getNum(string exp) {
	int length = 0;
	int isFirst = 1;
	int i = 0;

	length = exp.size();

	for (; i < length; i++) {
		char tmp = exp[i];
		int isNum = (tmp <= '9' && tmp >= '0') ? 1 : 0;
		if (isNum && isFirst) {
			num.push(tmp - 48);
			isFirst = 0;
		}
		else if (isNum) {
			num.top() *= 10;
			num.top() += tmp - 48;
		}
		else break;
	}
	if (exp[i] == '.') {
		i++;//跳过小数点
		i += getNum(&exp[i]);//获取小数点之后的完整数字，并将读取标记移动到数字之后
		for (; num.top() >= 1;) {//通过除以10操作将小数点后的数字转变为小数
			num.top() /= 10;
		}
		double Num = num.top();
		num.pop();
		num.top() += Num;//将存储的小数出栈，加到小数点前的整数上
	}
	return i;
}
//获取运算式中的运算符，并针对其优先级进行相应的操作
int getOp(char exp, int opType) {
	switch (exp) {
		//读到（，则将其入栈，并将当前堆栈最高优先级置为0
	case '(': {
		if (squareFlag > 0)squareFlag++;//对乘方符号的幂中的括号层数进行计数

		op.push(exp);
		opType = 0;
		nagNuFlag = 1;
		return opType;
		break;
	}
	case '+': {
		//读到+，且当前最高优先级小于‘+’的优先级，则将其入栈
		if (opType < 1) {
			op.push(exp);
			opType = 1;
			return opType;
		}
		//当前最高优先级大于、等于‘+’的优先级，则进入出栈操作流程
		else {
			for (; opType >= 1;) {
				getRes(op.top());//对出栈的运算符进行相应的运算操作
				op.pop();
				//获取当前运算符堆栈中的最高优先级
				if (op.size() > 0) {
					opType = getOpType(op.top());
				}
				else {
					opType = 0;
				}
			}
			//直到当前最高优先级小于‘+’的优先级，将其入栈
			op.push(exp);
			opType = 1;
			return opType;
		}
		break;
	}
//下面同理
	case '-': {
		if (opType < 1) {
			if (nagNuFlag == 1) {
				nagNuFlag = 0;
				op.push('N');
				opType = 3;
				return opType;
			}
			else {
				op.push(exp);
				opType = 1;
				return opType;
			}
		}
		else {
			for (; opType >= 1;) {
				getRes(op.top());
				op.pop();
				if (op.size() > 0) {
					opType = getOpType(op.top());
				}
				else {
					opType = 0;
				}

			}
			op.push(exp);
			opType = 1;
			return opType;
		}
		break;
	}
	case '*': {
		if (opType < 2) {
			op.push(exp);
			opType = 2;
			return opType;
		}
		else {
			for (; opType >= 2;) {
				getRes(op.top());
				op.pop();
				if (op.size() > 0) {
					opType = getOpType(op.top());
				}
				else {
					opType = 0;
				}
			}
			op.push(exp);
			opType = 2;
			return opType;
		}
		break;
	}
	case '/': {
		if (opType < 2) {
			op.push(exp);
			opType = 2;
			return opType;
		}
		else {
			for (; opType >= 2;) {
				getRes(op.top());
				op.pop();
				if (op.size() > 0) {
					opType = getOpType(op.top());
				}
				else {
					opType = 0;
				}
			}
			op.push(exp);
			opType = 2;
			return opType;
		}
		break;
	}  
	case ')': {//读到‘）’时，对运算符堆栈进行出栈处理，直到遇到‘（’
		if (squareFlag > 1)squareFlag--;

		for (; op.top() != '(';) {
			getRes(op.top());
			op.pop();
		}
		op.pop();
		if (op.size() > 0) {
			opType = getOpType(op.top());
		}
		else {
			opType = 0;
		}
		return opType;
		break;
	}
	case 'S': {
		op.push(exp);
		opType = 4;
		return opType;
	}
	}
	return opType;
}
//获取相应运算符的运算结果
void getRes(char op) {
	switch (op)
	{
	case '+': {
		double Num = num.top();
		num.pop();
		num.top() += Num;
		break;
	}
	case '-': {
		double Num = num.top();
		num.pop();
		num.top() -= Num;
		break;
	}
	case '*': {
		double Num = num.top();
		num.pop();
		num.top() *= Num;
		break;
	}
	case '/': {
		double Num = num.top();
		num.pop();
		num.top() /= Num;
		break;
	}
	case 'N': {
		num.top() = 0 - num.top();
		break;
	}
	case 'S': {
		num.top() = sqrt(num.top());
		break;
	}
	default:
		break;
	}
}
//获取当前运算堆栈中的最高优先级
int getOpType(char op) {
	switch (op) {
	case '+': case '-': return 1;
	case '*': case '/': return 2;
	case 'N':return 3;
	case 'S':return 4;
	default: return 0;
	}
}