#include <iostream>
using namespace std;
class Time
{
public:
	Time(int hour = 0)
	{
		cout << "Time" << endl;
		_hour = hour;
	}
private:
	int _hour;
};
class Test
{
public:
	// 在构造函数体内初始化（不使用初始化列表）
	Test(int hour)
	{
		Time t(hour);// 调用一次Time类的构造函数
		_t = t;// 调用一次Time类的赋值运算符重载函数
	}
private:
	Time _t;
};
int main()
{
	Test(12);
	return 0;
}