#include <iostream>
//#include "test_staticlib.h"
//#include "test_dynamiclib.h"

#include <map>
#include <sstream>

#include <mysql/mysql.h>
//#include <mysql.h>

#include <chrono>
using namespace std;
using namespace chrono;


int main()
{
    // cout<<"这是第一个案例"<<endl;
    // func_test_staticlib(111);
    // func_test_dynamiclib(222);

    MYSQL mysql;
    mysql_init(&mysql);
    //const char* host = "127.0.0.1";
	const char* host = "192.168.130.2";
	const char* user = "root";
	const char* pass = "123456";
	const char* db = "ubuntu_mysql";//数据库名称

	//CLIENT_MULTI_STATEMENTS 支持多条sql语句
	if (!mysql_real_connect(&mysql,host, user, pass, db, 3306, 0, CLIENT_MULTI_STATEMENTS))
	{
		cout << "mysql connect failed!" << mysql_error(&mysql) << endl;
	}
	else
	{
		cout << "mysql connect success!" << endl;
	}
	string sql = "";

	//1 创建表
	sql = "CREATE TABLE IF NOT EXISTS `t_image_2`  ( \
		`id` int AUTO_INCREMENT,\
		`name` varchar(1024),\
		`path` varchar(2046),\
		`size` int,\
		PRIMARY KEY(`id`)\
		) ENGINE=InnoDB;";  //设置支持事务回滚的 InnoDB引擎
	int re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
	}
	//清理表数据
	sql = "truncate t_image_2"; //清理是不能回滚的，所以不要放在事务中
	re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
	}


	//单条语句插入1千条数据	
	{
		auto start = system_clock::now();
		for (int i = 0; i < 1000; i++)
		{
			sql = "insert into t_image_2 (name,path) values('1 single','0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789')";
			re = mysql_query(&mysql, sql.c_str());
			if (re != 0)
			{
				cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
			}
		}
		auto end = system_clock::now();
		//转换为毫秒 1000
		auto dur = duration_cast<milliseconds> (end - start);
		cout << "1 单条语句插入1千条数据" << dur.count()/1000.<<"秒" << endl;
	}

	//多条语句插入1千条数据
	{
		auto start = system_clock::now();
		sql = "";
		//单条语句插入1千条数据
		for (int i = 0; i < 1000; i++)
		{
			sql += "insert into t_image_2 (name,path) values('2 single','0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789');";
		}

		re = mysql_query(&mysql, sql.c_str());
		if (re != 0)
		{
			cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
		}
		do{
			cout << mysql_affected_rows(&mysql) << flush;
		} while (mysql_next_result(&mysql) == 0);
		auto end = system_clock::now();
		//转换为毫秒 1000
		auto dur = duration_cast<milliseconds> (end - start);
		cout << "2 多条语句插入1千条数据" << dur.count() / 1000. << "秒" << endl;
	}

	//事务插入1千条数据
	{
		//1 开始事务
		//START TRANSACTION;
		sql = "START TRANSACTION";
		re = mysql_query(&mysql, sql.c_str());
		if (re != 0)
		{
			cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
		}
		auto start = system_clock::now();
		//单条语句插入1千条数据
		for (int i = 0; i < 1000; i++)
		{
			sql = "insert into t_image_2 (name,path) values('3 single','0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789')";
			re = mysql_query(&mysql, sql.c_str());
			if (re != 0)
			{
				cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
			}
			else
				cout << mysql_affected_rows(&mysql) << flush;

		}

		//2 设置为手动提交事务
		//set autocommit = 0
		sql = "set autocommit = 0";
		re = mysql_query(&mysql, sql.c_str());
		if (re != 0)
		{
			cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
		}

		sql = "COMMIT";
		re = mysql_query(&mysql, sql.c_str());
		if (re != 0)
		{
			cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
		}
		sql = "set autocommit = 1";
		re = mysql_query(&mysql, sql.c_str());
		if (re != 0)
		{
			cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
		}

		auto end = system_clock::now();
		//转换为毫秒 1000
		auto dur = duration_cast<milliseconds> (end - start);
		cout << "3 事务插入1千条数据" << dur.count() / 1000. << "秒" << endl;

	}

    mysql_close(&mysql);
	mysql_library_end();
    return 0;
}

//总结
/*
	事务快于多条，快于单条
*/