#include <iostream>
//#include "test_staticlib.h"
//#include "test_dynamiclib.h"

#include <map>
#include <sstream>

#include <mysql/mysql.h>
//#include <mysql.h>

using namespace std;


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

    //连接登录数据库
	if (!mysql_real_connect(&mysql,host,user,pass,db,3306,0,0))
	{
		cout << "mysql connect failed!" << mysql_error(&mysql) << endl;
	}
	else
	{
		cout << "mysql connect " << host << " success!" << endl;
	}

	// 自己创建一个数据库的一个表格
	string sql = "";
	//1.创建表
	sql = "CREATE TABLE IF NOT EXISTS `t_image_1`  ( \
		`id` int AUTO_INCREMENT,\
		`name` varchar(1024),\
		`path` varchar(2046),\
		`size` int,\
		PRIMARY KEY(`id`)\
		) ";
	int re = mysql_query(&mysql, sql.c_str());
	if (re!=0)
	{
		cout << "mysql_query failed!" << mysql_errno(&mysql) << endl;
	}

	// 1.1清空表格 truncate 清空数据，并恢复自增id从1开始
	sql = "truncate t_image_1";
	re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cout << "mysql_query failed!" << mysql_errno(&mysql) << endl;
	}

	// 2.往表格中插入1000条数据
	for (int i = 0; i < 10; i++)
	{
		//sql = "insert `t_image` (`name`,`path`,`size`) values ('test.jpg','d:/img/test.jpg',10240)";
		stringstream ss;	// 字符流
		ss << "insert `t_image_1` (`name`,`path`,`size`) values ('image";
		ss << i << ".jpg','d:/img/',10240)";
		sql = ss.str();		// 字符流转成字符串

		re = mysql_query(&mysql, sql.c_str());
		if (re == 0)
		{
			int count = mysql_affected_rows(&mysql);//可以不调用
			cout << "mysql_affected_rows " << count << ", id = " << mysql_insert_id(&mysql) << endl;
			// mysql_insert_id()的功能是获取插入数据的id
		}
		else
		{
			cout << "insert failed!" << mysql_errno(&mysql) << endl;
		}
	}

	// 3.根据map自动生成update sql语句
	map<string, string> kv;
	kv.insert(make_pair("name","image_update001.png"));
	kv.insert(make_pair("size", "5000"));
	string where = " where id=1;";
	string tmp = "";
	for (auto ptr = kv.begin(); ptr != kv.end(); ptr++)
	{
		tmp += "`";
		tmp += ptr->first;
		tmp += "`='";
		tmp += ptr->second;
		tmp += "',";
	}
	tmp += " id=id ";
	sql = "update t_image_1 set ";
	sql += tmp;
	sql += where;
	cout << sql << endl;

	re = mysql_query(&mysql, sql.c_str());
	if (re == 0)
	{
		int count = mysql_affected_rows(&mysql);//可以不调用
		cout << "update mysql_affected_rows " << count << endl;
	}
	else
	{
		cout << "update failed!" << mysql_errno(&mysql) << endl;
	}

	// 4.删除数据
	// delete不会实际删除空间，只做了标识
	// sql = "delete from t_image_1";
	sql = "delete from t_image_1 where id=2";
	re = mysql_query(&mysql, sql.c_str());
	if (re == 0)
	{
		int count = mysql_affected_rows(&mysql);//可以不调用
		cout << "delete mysql_affected_rows " << count << endl;
	}
	else
	{
		cout << "delete failed!" << mysql_errno(&mysql) << endl;
	}

    mysql_close(&mysql);

    return 0;
}