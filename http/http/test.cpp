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
	// �ڹ��캯�����ڳ�ʼ������ʹ�ó�ʼ���б�
	Test(int hour)
	{
		Time t(hour);// ����һ��Time��Ĺ��캯��
		_t = t;// ����һ��Time��ĸ�ֵ��������غ���
	}
private:
	Time _t;
};
int main()
{
	Test(12);
	return 0;
}