
#ifndef _XAgent_H__
#define _XAgent_H__

#include <iostream>
#include "LXMysql.h"
#include <string>

using namespace std;

class XAgent
{
public:
	static XAgent *Get()
	{
		static XAgent a;
		return &a;
	}
    //agent模块初始化，只能调用一次，在所有接口之前调用
	bool Init(std::string ip);

    //主循环
	void Main();
	
	//日志写入数据库
	bool SaveLog(std::string log);

	//获取本机的IP地址
	static std::string GetLocalIp();

	~XAgent();
private:
	XAgent();
    //数据库对象
    LX::LXMysql *my = 0;

    //打开日志文件
	FILE *fp = 0;

	std::string local_ip;
};


#endif