#include <iostream>
//#include "test_staticlib.h"
//#include "test_dynamiclib.h"

#include <map>
#include <sstream>

#include <mysql/mysql.h>
//#include <mysql.h>
#include <fstream>

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

	//1 创建存储过程    存储过程就是函数
	sql = "CREATE PROCEDURE `p_test` (IN p_in INT,OUT p_out INT,INOUT  p_inout INT)\
	BEGIN\
	SELECT p_in,p_out,p_inout;\
	SET p_in = 100, p_out = 200,p_inout=300; \
	SELECT p_in,p_out,p_inout;\
	END";
	int re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cout << mysql_error(&mysql) << endl;
	}

	//2 定义变量并复制，参数
	cout << "IN in=1 out=2 inout=3" << endl;
	sql = "SET @A=1;SET @B=2;SET @C=3;";
	re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cout << mysql_error(&mysql) << endl;
	}
	//遍历（清空）结果集
	do
	{
		cout << "SET affect " << mysql_affected_rows(&mysql) << endl;
	}
	while (mysql_next_result(&mysql) == 0);//0 还有结果， -1 没有结果 >1错误

	//3 调用存储过程 call 
	sql = "call p_test(@A,@B,@C)";
	re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cout << mysql_error(&mysql) << endl;
	}
	cout << "In Proc:"; //在存储过程中
	do
	{
		MYSQL_RES *res = mysql_store_result(&mysql);//把结果集存储
		if (!res)
			continue;

		//字段数量
		int fcount = mysql_num_fields(res);

		//打印结果集
		for (;;)
		{
			//提取一行记录
			MYSQL_ROW row = mysql_fetch_row(res); //遍历结果集
			if (!row) break; //取不到结果的时候就退出for循环
			for (int i = 0; i < fcount; i++)
			{
				if (row[i])
				{
					cout << row[i] << " ";
				}
				else
					cout << "NULL"<< " ";
			}
			cout << endl;

		}
		mysql_free_result(res); // 不释放是有内存泄露的
	}
	//0 还有结果， -1 没有结果 >1错误
	while (mysql_next_result(&mysql) == 0);


	//4 获取存储过程的结果
	sql = "select @A,@B,@C";
	re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cout << mysql_error(&mysql) << endl;
	}
	MYSQL_RES *res = mysql_store_result(&mysql);
	cout << "out: ";
	MYSQL_ROW row = mysql_fetch_row(res);
	cout << " in=" << row[0];
	cout << " out=" << row[1];
	cout << " inout=" << row[2];
	mysql_free_result(res);
	cout << endl;


    mysql_close(&mysql);
	mysql_library_end();
    return 0;
}

