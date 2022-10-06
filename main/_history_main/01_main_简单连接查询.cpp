#include <iostream>
#include "test_staticlib.h"
#include "test_dynamiclib.h"

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
	const char* db = "testdb";//数据库名称

    //连接登录数据库
	if (!mysql_real_connect(&mysql,host,user,pass,db,3306,0,0))
	{
		cout << "mysql connect failed!" << mysql_error(&mysql) << endl;
	}
	else
	{
		cout << "mysql connect " << host << " success!" << endl;
	}

    // 1 执行SQL语句
    const char* sql = "select cust_name, cust_state from custnew where cust_id > 10003";
    int re = mysql_query(&mysql, sql);
    if (re != 0)
	{
		cout << "mysql_real_query failed! " << sql << " " << mysql_error(&mysql) << endl;
	}
	else
	{
		cout << "mysql_real_query success! " << sql << endl;
	}

    //2 获取结果集
	MYSQL_RES* result = mysql_store_result(&mysql);
	if (!result)
	{
		cout << "mysql_use_result failed!" << mysql_errno(&mysql);
	}

    //3 遍历结果集
	MYSQL_ROW row;
	while ((row=mysql_fetch_row(result)) != nullptr)
	{
		unsigned long * lens=mysql_fetch_lengths(result);
		//cout << lens[0] <<"[" << row[0] << "," << row[1] << "]" << endl;
        cout << lens[0] << "\t" << lens[1] << "\t" <<"[" << row[0] << ", \t" << row[1] << "]" << endl;
	}

    mysql_close(&mysql);

    return 0;
}