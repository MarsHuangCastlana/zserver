#pragma once
#include <list>
#include <string>
#include <vector>

using namespace std;

//�պ�    ����ɵ����Ա�
class FirstName {
public:
	std::string m_first;

	//ʹ��vector
	std::vector<std::string> m_last_list;
};

class RandomName
{
	std::vector<FirstName*> m_pool;
public:
	RandomName();
	std::string GetName();
	void Release(std::string _name); //������
	void LoadFile(); //ͨ���ļ�������
	virtual ~RandomName();
};


