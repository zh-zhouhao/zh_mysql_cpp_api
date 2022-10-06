
#ifndef _XCenter_H__
#define _XCenter_H__

#include <iostream>
#include "LXMysql.h"
#include <string>

using namespace std;
using namespace LX;

class XCenter
{
public:
	static XCenter* Get()
	{
		static XCenter c;
		return &c;
	}

	bool Install(string ip);
	//初始化数据库
	bool Init();
	//添加设备
	bool AddDevice(string ip,string name);
	//主循环函数
	void Main();

	~XCenter();

private:
	XCenter();

	LXMysql* my;
};

#endif