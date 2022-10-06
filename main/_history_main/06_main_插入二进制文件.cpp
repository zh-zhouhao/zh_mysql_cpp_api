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

	//1 创建好存放二进制数据的表 t_data
	sql = "CREATE TABLE IF NOT EXISTS `t_data` (\
		`id` int AUTO_INCREMENT,\
		`name` varchar(1024),\
		`data` blob,\
		`size` int,\
		PRIMARY KEY(`id`))";

	int re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cout << mysql_error(&mysql) << endl;
	}

	//2 清空表 truncate t_data
	sql = "truncate t_data";
	re = mysql_query(&mysql, sql.c_str());
	if (re != 0)
	{
		cerr << mysql_error(&mysql) << endl;
	}

	//3 初始化stmt mysql_stmt_init
	MYSQL_STMT *stmt = mysql_stmt_init(&mysql);
	if (!stmt)
	{
		cerr << "mysql_stmt_init failed!" << mysql_error(&mysql) << endl;
	}

	//4 预处理sql语句
	sql = "INSERT INTO `t_data` (name,data,size) VALUES(?,?,?)";
	if (mysql_stmt_prepare(stmt, sql.c_str(), sql.size()))
	{
		cerr << "mysql_stmt_prepare failed!" << mysql_stmt_error(stmt) << endl;
	}

	//5 打开并读取文件
	string filename = "../_history/06_mysql.jpg";
	
	//读取二进制
	fstream in(filename, ios::in | ios::binary); // 读入|二进制
	if (!in.is_open())
	{
		cerr << "file " << filename << " open failed!" << endl;
	}
	//文件指针移动到结尾处
	in.seekg(0, ios::end);
	//文件大小和文件二进制地址
	int filesize = in.tellg();
	//回到开头
	in.seekg(0, ios::beg);

	char *data = new char[filesize];
	int readed = 0; //已经读了多少
	while (!in.eof())//循环读
	{
		in.read(data+ readed, filesize- readed);
		//读取了多少字节
		if (in.gcount() <= 0)
			break;
		readed += in.gcount();
	}
	in.close();//资源释放

	//6 绑定字段
	MYSQL_BIND bind[3] = { };
	bind[0].buffer_type = MYSQL_TYPE_STRING;  //name 文件名
	bind[0].buffer = (char*)filename.c_str(); 
	bind[0].buffer_length = filename.size();

	bind[1].buffer_type = MYSQL_TYPE_BLOB;   //data 文件二进制内容
	bind[1].buffer = data; //二进制文件
	bind[1].buffer_length = filesize;

	bind[2].buffer_type = MYSQL_TYPE_LONG;	//文件大小
	bind[2].buffer = &filesize;

	if (mysql_stmt_bind_param(stmt, bind) != 0)
	{
		cerr << "mysql_stmt_bind_param failed! " << mysql_stmt_error(stmt) << endl;
	}

	//7 执行stmt sql
	if (mysql_stmt_execute(stmt) != 0)
	{
		cerr << "mysql_stmt_execute failed! " << mysql_stmt_error(stmt) << endl;
	}
	delete data;
	mysql_stmt_close(stmt);


    mysql_close(&mysql);
	mysql_library_end();
    return 0;
}
