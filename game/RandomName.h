#pragma once
#include <list>
#include <string>
#include <vector>

using namespace std;

//姓和    名组成的线性表
class FirstName {
public:
	std::string m_first;

	//使用vector
	std::vector<std::string> m_last_list;
};

class RandomName
{
	std::vector<FirstName*> m_pool;
public:
	RandomName();
	std::string GetName();
	void Release(std::string _name); //还名字
	void LoadFile(); //通过文件来构造
	virtual ~RandomName();
};


