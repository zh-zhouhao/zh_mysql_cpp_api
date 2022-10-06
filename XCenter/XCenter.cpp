#include <iostream>
#include <thread>
#include "LXMysql.h"
#include "XCenter.h"
#include <string.h>
#include <fstream>

#include <regex> //正则表达式，C++11中的

using namespace std;
using namespace LX;

#define CENTER_CONF "ip"

void XCenter::Main()
{
	//只审计运行之后的事件
	//找到最后一个事件，取到id号
	int lastid = 0;
	//返回多行记录，简易接口
	auto rows = my->GetResult("select max(id) from t_log");
	if (rows[0][0].data) //第一行
	{
		lastid = atoi(rows[0][0].data); //id
	}
	cout << "last id is " << lastid << endl;

	/*
	//取到审计策略
	//string sql = "CREATE TABLE `t_strategy` (\
	//				`id` INT AUTO_INCREMENT,\
	//				`name` VARCHAR(256) CHARACTER SET 'utf8' COLLATE 'utf8_bin',\
	//				`strategy` VARCHAR(4096), \
	//				PRIMARY KEY(`id`))";
	*/
	rows = my->GetResult("select * from t_strategy");
	//正则表达式map key 审计事件名称
	map<string, regex> strategys;
	for (auto row : rows)
	{
		if (row[1].data && row[2].data) // 不要是空的
			//初始化正则
			strategys[row[1].data] = regex(row[2].data);
	}

	for (;;)
	{
		char buf[1024] = { 0 };

		//_CRT_SECURE_NO_WARNINGS

		//获取agent存储最新数据
		sprintf(buf, "select * from t_log where id>%d", lastid);
		auto rows = my->GetResult(buf);
		if(rows.empty())
		{
			std::chrono::milliseconds dura(100);//100ms
			this_thread::sleep_for(dura);
		}
		/*
		//表结构：log 在的位置
		//sql = "CREATE TABLE IF NOT EXISTS `t_log` (\
		//		`id` INT AUTO_INCREMENT,\
		//		`ip` VARCHAR(16),\
		//		`log` VARCHAR(2048) ,\
		//	    `log_time` datetime,\
		//		PRIMARY KEY(`id`))";
		*/
		//遍历日志列表
		for (auto row : rows)
		{
			lastid = atoi(row[0].data);//更新lastid
			if (!row[2].data)
				continue;
			cout << row[2].data << endl;
			for (auto strategy : strategys)
			{
				//正则结果
				smatch match;
				string data = row[2].data;
				//匹配正则，返回结果到match
				bool ret = regex_match(data, match, strategy.second);
				if (!ret || match.size() <= 0) // 没有匹配上
				{
					continue;
				}
				//审计成功打印下策略名称
				cout << strategy.first.c_str() << endl;
				/*
				//sql = "CREATE TABLE IF NOT EXISTS `t_audit` (\
				//`id` INT AUTO_INCREMENT,\
				//`name` VARCHAR(256), \
				//`context` VARCHAR(2048), \
				//`user` VARCHAR(256), \
				//`device_ip` VARCHAR(16),\
				//`from_ip` VARCHAR(16),\
				//`port` int,\
			 	//`last_heart` datetime,\
				//PRIMARY KEY(`id`))";
				*/
				XDATA d;
				//审计成功的，事件名称
				d["name"] = strategy.first.c_str();
				d["context"] = data.c_str();
				if (row[1].data) // 设备ip
					d["device_ip"] = row[1].data;
				//匹配结果， 下标0 是整个字符串 1是第一个匹配结果
				string user = match[1];
				string from_ip = match[2];
				string port = match[3];
				d["user"] = user.c_str();
				d["from_ip"] = from_ip.c_str();
				d["port"] = port.c_str();
				my->Insert(d, "t_audit");

			}
		}
	}
}



//添加设备
bool XCenter::AddDevice(std::string ip, std::string name)
{
	/*
	my->Query("DROP TABLE IF EXISTS `t_device`");
	"CREATE TABLE IF NOT EXISTS `t_device` (\
				`id` INT AUTO_INCREMENT,\
				`ip` VARCHAR(16),\
				`name` VARCHAR(2048) ,\
			    `last_heart` datetime,\
				PRIMARY KEY(`id`))";
	*/
	//根据设备表表结构插入数据

	XDATA data;
	data["ip"] = ip.c_str();
	data["name"] = name.c_str();
	return my->Insert(data,"t_device");
}


//初始化数据库
bool XCenter::Init()
{
	my = new LXMysql();
	string ip = "";
	//读取数据库ip配置
	ifstream fs;
	fs.open(CENTER_CONF);
	if(!fs.is_open())
	{
		cout << "please install center" << endl;
		return false;
	}
	fs >> ip; // 读取
	fs.close();
	if (ip.empty())
	{
		cout << "please install center" << endl;
		return false;
	}
	cout << "Init center " << ip << endl;

	if (!my->Connect(ip.c_str(), "root", "123456", "ubuntu_mysql"))
	{
		cout << "db connect failed!" << endl;
		return false;
	}
	cout << "db connect success!" << endl;
	return my->Query("set names utf8");
}

bool XCenter::Install(string ip)
{
	// 1 生成配置文件 数据库的IP
	ofstream of;
	of.open(CENTER_CONF);
	if (!of.is_open())
	{
		cout << "open conf " << CENTER_CONF << " failed!" << endl;
		return false;
	}
	of << ip; // 写入ip到文件中
	of.close();

	//初始化表和数据
	if (!Init())
		return false;

	cout << "XCenter::Install()" << ip << endl;

	my->StartTransaction();
	//创建策略表
	//清除原来数据，防止数据污染
	my->Query("DROP TABLE IF EXISTS `t_strategy`");
	string sql = "CREATE TABLE `t_strategy` (\
					`id` INT AUTO_INCREMENT,\
					`name` VARCHAR(256) CHARACTER SET 'utf8' COLLATE 'utf8_bin',\
					`strategy` VARCHAR(4096), \
					PRIMARY KEY(`id`))";
	bool re = my->Query(sql.c_str());
	if (!re)
	{
		my->Rollback();
		return false;
	}
	{
		//Jan  7 19:23:30 ubuntu sshd[16830]: Failed password for xcj from 192.168.0.202 port 38944 ssh2
		XDATA data;
		data["name"] = u8"用户登录失败";
		data["strategy"] = ".*Failed password for (.+) from ([0-9.]+) port ([0-9]+).*";
		my->Insert(data,"t_strategy");


		//Jan  2 21:31 : 09 ubuntu sshd[43681]: Accepted password for xcj from 192.168.3.89 port 44602 ssh2
		data["name"] = u8"用户登录成功";
		data["strategy"] = ".*Accepted password for (.+) from ([0-9.]+) port ([0-9]+).*";
		my->Insert(data, "t_strategy");

	}

	//创建用户表，初始化管理员用户 root 123456 md5
	// t_user id user pass
	my->Query("DROP TABLE IF EXISTS `t_user`");
	sql = "CREATE TABLE `t_user` (\
					`id` INT AUTO_INCREMENT,\
					`user` VARCHAR(256) CHARACTER SET 'utf8' COLLATE 'utf8_bin',\
					`pass` VARCHAR(1024), \
					PRIMARY KEY(`id`))";
	re = my->Query(sql.c_str());
	if (!re)
	{
		my->Rollback();
		return false;
	}
	{
		XDATA data;
		data["user"] = "root";
		data["@pass"] = "md5('123456')"; // md5()函数
		my->Insert(data, "t_user");
	}

	//创建日志表 t_log
	my->Query("DROP TABLE IF EXISTS `t_log`");
	sql = "CREATE TABLE IF NOT EXISTS `t_log` (\
				`id` INT AUTO_INCREMENT,\
				`ip` VARCHAR(16),\
				`log` VARCHAR(2048) ,\
			    `log_time` datetime,\
				PRIMARY KEY(`id`))";
	my->Query(sql.c_str());

	///创建设备表Agent t_device
	my->Query("DROP TABLE IF EXISTS `t_device`");
	sql = "CREATE TABLE IF NOT EXISTS `t_device` (\
				`id` INT AUTO_INCREMENT,\
				`ip` VARCHAR(16),\
				`name` VARCHAR(2048) ,\
			    `last_heart` datetime,\
				PRIMARY KEY(`id`))";
	my->Query(sql.c_str());

	///创建审计结果 t_audit
	my->Query("DROP TABLE IF EXISTS `t_audit`");
	sql = "CREATE TABLE IF NOT EXISTS `t_audit` (\
				`id` INT AUTO_INCREMENT,\
				`name` VARCHAR(256), \
				`context` VARCHAR(2048), \
				`user` VARCHAR(256), \
				`device_ip` VARCHAR(16),\
				`from_ip` VARCHAR(16),\
				`port` int,\
			    `last_heart` datetime,\
				PRIMARY KEY(`id`))";
	my->Query(sql.c_str());
/**/
	my->Commit();
	my->StopTransaction();

	return true;
}

XCenter::XCenter()
{
	// cout<<"XCenter::XCenter()"<<endl;
}

XCenter::~XCenter()
{
	// cout<<"XCenter::~XCenter()"<<endl;
}
