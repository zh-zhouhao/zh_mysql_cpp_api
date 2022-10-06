
#ifndef _XClient_H__
#define _XClient_H__

#include <iostream>
#include "LXMysql.h"
#include <string>

using namespace std;
using namespace LX;

class XClient
{
public:
public:
	static XClient *Get()
	{
		static XClient c;
		return &c;
	}
	bool Init(std::string ip);

	//主循环函数
	void Main();
	//用户登录
	bool Login();

	string InputPassword();

	std::string Input();

	

	//检查用户输入，防止注入攻击 true表示安全
	bool CheckInput(const std::string &in);

	~XClient();

	//最大登录失败次数
	int maxLoginTimes = 10;

private:

	
	//log 1 10 第一页，每页十条
	void c_log(std::vector<std::string>cmds);

	//审计结果显示 audit 
	void c_audit(std::vector<std::string>cmds);

	// test 10000 插入一万条测试数据
	void c_test(std::vector<std::string>cmds);
	
	//search 127.0.0.1 搜索ip
	void c_search(std::vector<std::string>cmds);

	//like 搜索ip，和日志内容 like
	void c_like(std::vector<std::string>cmds);

	XClient();
	LX::LXMysql *my = 0;
};

#endif