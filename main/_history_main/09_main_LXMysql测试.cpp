#include <iostream>
// #include "test_staticlib.h"
// #include "test_dynamiclib.h"
#include <chrono>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <mysql/mysql.h>

#include "LXMysql.h" //自己封装的 LXMysql
#include "LXData.h"

#include <thread>

using namespace std;
using namespace chrono;
using namespace LX;

int main()
{
    // cout<<"这是第一个案例"<<endl;
    // func_test_staticlib(111);
    // func_test_dynamiclib(222);

    LXMysql my;
    //1 mysql 初始化
    cout << "my.Init() = " << my.Init() << endl;

    my.SetConnectTimeout(3); //连接超时秒
    my.SetReconnect(true);   //自动重连

    //2 连接mysql 如果没有调用Init 内部会自动调用
    if (my.Connect("192.168.130.2", "root", "123456", "ubuntu_mysql"))
    {
        cout << "my.Connect success！" << endl;
    }

    //3 执行sql语句创建表
    string sql = "";

    sql = "CREATE TABLE IF NOT EXISTS `t_video` \
			(`id` INT AUTO_INCREMENT, \
			`name` VARCHAR(1024) , \
			`data` BLOB,\
			`size` INT,\
            PRIMARY KEY(`id`))";

    cout << my.Query(sql.c_str()) << endl;

    //清空数据，包括自增id
    sql = "TRUNCATE t_video";
    my.Query(sql.c_str());

    // //插入一条记录
    // sql = "insert into t_video(name) values('test001')";
    // cout << my.Query(sql.c_str()) << endl;
    // cout << my.Query(sql.c_str()) << endl;
    // cout << my.Query(sql.c_str()) << endl;
    // cout << my.Query(sql.c_str()) << endl;
    // cout << my.Query(sql.c_str()) << endl;
    // cout << my.Query(sql.c_str()) << endl;
    XDATA data1;

    // data1.insert(make_pair("name", LXData("insertname001")));
    data1["name"] = LXData("insertname001");
    data1["size"] = LXData("1024");
    //cout << my.GetInsertSql(data1, "t_video") << endl;
    my.Insert(data1, "t_video");

    data1["name"] = LXData("insertname002");
    data1["size"] = LXData("1024");
    my.Insert(data1, "t_video");

    //mysql.jpg
    data1["name"] = "mysql_new1.jpg";
    //data1["size"] =
    LXData file1;
    file1.LoadFile("mysql.jpg");
    data1["data"] = file1;
    data1["size"] = &file1.size;
    my.InsertBin(data1, "t_video"); // 插入二进制文件到数据库中

    data1["name"] = "mysql_new2.jpg";
    my.InsertBin(data1, "t_video");
    data1["name"] = "mysql_new3.jpg";
    my.InsertBin(data1, "t_video");

    file1.Drop(); // 要把里面的空间清理掉
                  //my.Insert(data1, "t_video");

    //修改id=1的普通数据
    XDATA udata;
    udata["name"] = "updatename001";
    cout << "my.Update = " << my.Update(udata, "t_video", "where id=1") << endl;

    //修改id=5的二进制数据（图片）
    XDATA udata2;
    LXData file2;
    file2.LoadFile("mysql2.jpg");
    udata2["name"] = "mysql2.jpg";
    udata2["data"] = file2;
    cout << "my.UpdateBin = " << my.UpdateBin(udata2, "t_video", "where id=5") << endl;
    file2.Drop();

    //使用事务插入多条记录
    XDATA data3;
    data3["name"] = "trans001";
    data3["size"] = "1024";
    my.StartTransaction();
    my.Insert(data3, "t_video");
    my.Insert(data3, "t_video");
    my.Insert(data3, "t_video");
    my.Rollback(); //回滚

    data3["name"] = "trans002";
    my.Insert(data3, "t_video");

    data3["name"] = "trans003";
    my.Insert(data3, "t_video");
    my.Commit();
    my.StopTransaction();

    //获取结果集
    sql = "select * from t_video";
    cout << my.Query(sql.c_str()) << endl;
    my.StoreResult(); //结果集本地全部存储

    for (;;)
    {
        //获取一行数据
        auto row = my.FetchRow(); // 返回值类型 std::vector<LXData>
        if (row.size() == 0)
            break;
        row[2].SaveFile(row[1].data);

        for (uint i = 0; i < row.size(); i++)
        {
            if (row[i].data) // 有可能是空，所以做一下判断
                cout << row[i].data << " ";
        }
        cout << endl;
    }

    my.FreeResult();

    cout << my.Query(sql.c_str()) << endl;
    my.UseResult(); //开始接收结果集
    my.FreeResult();

    // //测试自动重连
    // for (;;)
    // {
    // 	cout << my.Query(sql.c_str())<<flush;
    // }

    //开始测试字符集问题，插入、读取 GBK  UTF-8
    cout << "开始测试字符集" << endl;
    //测试utf-8
    sql = "CREATE TABLE IF NOT EXISTS `t_utf8` \
		(`id` INT AUTO_INCREMENT,	\
		`name` VARCHAR(1024) CHARACTER SET utf8 COLLATE utf8_bin,\
		PRIMARY KEY(`id`))";
    /**
     * utf8 : 字符集
     * utf8_bin : 字符集的排序方式
    */
    my.Query(sql.c_str());
    //清空数据
    my.Query("TRUNCATE t_utf8");
    //指定与mysql处理的字符集
    my.Query("set names utf8"); // 设置编码
    {
        XDATA data;
        data["name"] = u8"测试的UTF中文";
        my.Insert(data, "t_utf8");
    }

    //插入gbk的数据
    sql = "CREATE TABLE IF NOT EXISTS `t_gbk` \
		(`id` INT AUTO_INCREMENT,	\
		`name` VARCHAR(1024) CHARACTER SET gbk COLLATE gbk_bin,\
		PRIMARY KEY(`id`))";
    my.Query(sql.c_str());
    //清空数据
    my.Query("TRUNCATE t_gbk");
    //指定与mysql处理的字符集
    my.Query("set names gbk");
    {
        XDATA data;
        LXData tmp = u8"测试的GBK中文"; //Linux默认是 utf-8，要是把utf-8的字符串直接插入到一个字符集位gbk的表时，会出现乱码
        string gbk = tmp.UTF8ToGBK();   //把这个tmp从默认的utf-8转成gbk格式，在插入到字符集为gbk的表格中
        data["name"] = gbk.c_str();
        my.Insert(data, "t_gbk");
    }

    cout << "==== Print names gbk string ==== " << endl;
    my.Query("set names gbk"); // 设置编码
    my.Query("select * from t_gbk");
    my.StoreResult();
    for (;;)
    {
        //获取一行数据
        auto row = my.FetchRow();
        if (row.size() == 0)
            break;
#ifdef _WIN32
        cout << "id:" << row[0].data << " name:" << row[1].data << endl;
#else
        cout << "id:" << row[0].data << " name:" << row[1].GBKToUTF8() << endl; // 在Linux下的字符集是utf8,在从数据库gbk的表下来的数据，不经转换的显示会有乱码
#endif
    }
    my.FreeResult();

    cout << "==== Print utf-8 string ==== " << endl;
    my.Query("set names utf8");
    my.Query("select * from t_utf8");
    my.StoreResult();
    for (;;)
    {
        //获取一行数据
        auto row = my.FetchRow();
        if (row.size() == 0)
            break;

#ifdef _WIN32
        //cout << "id:" << row[0].data << " name:" << row[1].data << endl; // 在window下的字符集是gbk,在从数据库utf-8的表下来的数据，不经转换的显示会有乱码
        cout << "id:" << row[0].data << " name:" << row[1].UTF8ToGBK() << endl;
#else
        cout << "id:" << row[0].data << " name:" << row[1].data << endl;
#endif
    }
    my.FreeResult();

    cout << "----------简易获取数据的接口GetResult实现-----------" << endl;

    XROWS rows = my.GetResult("select * from t_video");
    for (uint i = 0; i < rows.size(); i++)
    {
        auto row = rows[i];
        for (uint i = 0; i < row.size(); i++)
        {
            if (!row[i].data)
            {
                cout << "[NULL],";
                continue;
            }
            switch (row[i].type) //只判断使用了的类型
            {
            case LX_TYPE_BLOB:
                cout << "[BLOB]";
                break;
            case LX_TYPE_LONG:
            case LX_TYPE_STRING:
            default:
                cout << row[i].data;
                break;
            }

            cout << ",";
        }
        cout << endl;
    }

    cout << "----------mysql的表锁和行锁代码示例购票竞争-----------" << endl;
    //订票模拟(事务) t_tickets(id int,sold int)
	//插入gbk的数据
	sql = "CREATE TABLE IF NOT EXISTS `t_tickets` \
		(`id` INT AUTO_INCREMENT,	\
		`sold` INT,\
		PRIMARY KEY(`id`))";
	my.Query(sql.c_str());

	{
		XDATA data;
		data["sold"] = "0";
		my.Insert(data, "t_tickets"); //id=1
		my.StartTransaction();

		//行锁
		XROWS rows = my.GetResult("select * from t_tickets where sold=0 order by id for update");//
        // 加了 update 表示对操作的数据加锁了
		string id = rows[0][0].data;
		cout << "Buy ticket id is " << id << endl;

		//模拟冲突
        std::chrono::milliseconds dura(10000);  //10s,sleep_for不直接传数值
		this_thread::sleep_for(dura);
		data["sold"] = "1";
		string where = "where id=";
		where += id;
		cout<<my.Update(data, "t_tickets", where)<<endl;

		cout << "Buy ticket id  " << id  <<" success!"<< endl;
		//my.GetResult("select * from t_tickets where sold=0 for update");
		my.Commit();
		my.StopTransaction();
	}

    my.Close();
    std::cout << "test_LXMysql!\n";

    return 0;
}
