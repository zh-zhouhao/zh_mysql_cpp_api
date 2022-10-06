#include <iostream>
#include <thread>
#include "LXMysql.h"
#include "XAgent.h"
#include <string.h>

#ifndef _WIN32
#include <ifaddrs.h>
#include <arpa/inet.h>
#endif

using namespace std;
using namespace LX;

#define LOGPATH "/var/log/auth.log"

std::string XAgent::GetLocalIp()
{
	char ip[16] = {0};
#ifndef _WIN32
	ifaddrs *ifadd = 0;
	if (getifaddrs(&ifadd) != 0)
		return "";
	//遍历地址
	ifaddrs *iter = ifadd;

	while (iter != NULL)
	{
		// ipv4
		if (iter->ifa_addr->sa_family == AF_INET)
			if (strcmp(iter->ifa_name, "lo") != 0) //去掉回环地址 127.0.0.1
			{
				//转换整形ip为字符串

				void *tmp = &((sockaddr_in *)iter->ifa_addr)->sin_addr;
				inet_ntop(AF_INET, tmp, ip, INET_ADDRSTRLEN);
				break;
			}
		iter = iter->ifa_next;
	}
	freeifaddrs(ifadd);
#endif
	return ip;
}

// agent模块初始化，只能调用一次，在所有接口之前调用
bool XAgent::Init(string ip)
{
	local_ip = GetLocalIp();
	// local_ip = GetLocalIp();//一开始就调用
	if (ip.empty())
	{
		cerr << "XAgent::Init failed! ip is empty!" << endl;
		return false;
	}
	my = new LXMysql();
	//连接数据库
	//这里的 数据库名，用户名，密码 是写死的啦
	if (!my->Connect(ip.c_str(), "root", "123456", "ubuntu_mysql"))
	{
		cerr << "XAgent::Init failed! Connect DB failed!" << endl;
		return false;
	}

	//读取日志文件
	fp = fopen(LOGPATH, "rb");
	if (!fp)
	{
		cerr << "open log " << LOGPATH << " failed!" << endl;
		return false;
	}
	cout << "open log " << LOGPATH << " success!" << endl;
	//只审计系统开始运行之后事件

	//文件移动到结尾
	fseek(fp, 0, SEEK_END);

	// 创建日志表，在其他地方

	return true;
}

//日志写入数据库
bool XAgent::SaveLog(std::string log)
{
	cout << log << endl;
	XDATA data;
	data["log"] = log.c_str();
	data["ip"] = local_ip.c_str();

	//插入时间，用mysql now（）
	//@表示 字段内容不加引号，@会自动去除
	data["@log_time"] = "now()";
	my->Insert(data,"t_log");

	return true;
}

//主循环
void XAgent::Main()
{
	//读取最新的日志
	string log = "";
	for (;;)
	{
		char c = fgetc(fp);
		//耗费cpu
		if (c <= 0) //没有字符或者读到结尾
		{
			std::chrono::milliseconds dura(100);
			this_thread::sleep_for(dura);
			continue;
		}
		if (c == '\n') //换行
		{
			SaveLog(log);
			log = "";
			continue;
		}
		log += c;
	}
}

XAgent::XAgent()
{
	// cout<<"1111111111111"<<endl;
}

XAgent::~XAgent()
{
	// cout<<"2222222222222"<<endl;
}
