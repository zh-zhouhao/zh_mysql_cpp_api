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
	if (!mysql_real_connect(&mysql,host,user,pass,db,3306,0,CLIENT_MULTI_STATEMENTS))
	// if (!mysql_real_connect(&mysql,host,user,pass,db,3306,0,0))
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
		) ;";

	// 1.1清空表格 truncate 清空数据，并恢复自增id从1开始
	sql += "truncate t_image_1;";	//ps: 注意这个分号的种类

	// 2.往表格中插入1000条数据
	for (int i = 0; i < 5; i++)
	{
		//sql = "insert `t_image` (`name`,`path`,`size`) values ('test.jpg','d:/img/test.jpg',10240)";
		stringstream ss;	// 字符流
		ss << "insert `t_image_1` (`name`,`path`,`size`) values ('image";
		ss << i << ".jpg','d:/img/',10240);";
		sql += ss.str();		// 字符流转成字符串
	}

	//3 修改数据
	sql += "update t_image_1 set `name`='test3.png',size=2000 where id=1;";
	//4 删除数据
	sql += "delete from t_image_1 where id=2;";
	//
	sql += "select * from t_image_1;";

	//执行sql语句立刻返回，但语句并没有全部执行好，需要获取结果
	//把sql整个发送给mysql server，server一条条执行，返回结果
	int re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cout << "mysql_query failed!" << mysql_error(&mysql) << endl;
	}

	//有多个返回结果
	do
	{
		cout << "[result]";
		MYSQL_RES * result = mysql_store_result(&mysql);
		if (result) //SELECT
		{
			cout << "SELECT mysql_num_rows = " << mysql_num_rows(result) << endl;
			mysql_free_result(result); //释放结果集
		}
		else  // INSERT UPDATE DELETE CREATE DROP truncate
		{
			// SELECT 出错 有字段无结果
			if (mysql_field_count(&mysql) > 0)
			{
				cout << "Not retrieve result! " <<mysql_error(&mysql)<< endl;
			}
			else //INSERT UPDATE DELETE CREATE DROP truncate
			{
				//等待服务器的处理结果
				cout << mysql_affected_rows(&mysql) << " rows affected!" << endl;
			}
		}
	}
	while (mysql_next_result(&mysql) == 0);//取下一条结果 0表示有结果

    mysql_close(&mysql);
	mysql_library_end();
    return 0;
}