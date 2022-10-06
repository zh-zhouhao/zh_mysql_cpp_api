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

	//CLIENT_MULTI_STATEMENTS 支持多条sql语句
	if (!mysql_real_connect(&mysql,host, user, pass, db, 3306, 0, 0))
	//if (!mysql_real_connect(&mysql, host, user, pass, db, 3306, 0, 0))
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

	// 事务
	//1 开始事务
	//START TRANSACTION;
	sql = "START TRANSACTION";
	re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
	}

	//2 设置为手动提交事务
	//set autocommit = 0
	sql = "set autocommit = 0";
	re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
	}

	//3 sql语句

	//插入三条数据，回滚
	for (int i = 0; i < 3; i++)
	{
		sql = "insert into t_image_2 (name) values('test three!')";
		re = mysql_query(&mysql, sql.c_str());
		if (re != 0)
		{
			cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
		}
	}

	//4 回滚ROLLBACK MYISAM 不支持
	sql = "ROLLBACK";
	re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
	}
	for (int i = 0; i < 1000; i++)
	{
		sql = "insert into t_image_2 (name) values('test three!')";
		re = mysql_query(&mysql, sql.c_str());
		if (re != 0)
		{
			cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
		}
	}

	//5 COMMIT  
	sql = "COMMIT";
	re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
	}

	//6 恢复自动提交 set autocommit = 1
	sql = "set autocommit = 1";
	re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
	}


	sql = "select count(*) from t_image_2";
	re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cout << "mysql_query failed! " << mysql_error(&mysql) << endl;
	}
	MYSQL_RES * res = mysql_store_result(&mysql);
	if (res)
	{
		//取得第一行数据
		MYSQL_ROW row = mysql_fetch_row(res);
		if (row)
		{
			cout << "t_image_2 count(*) = " << row[0] << endl;
		}
	}

    mysql_close(&mysql);
	mysql_library_end();
    return 0;
}