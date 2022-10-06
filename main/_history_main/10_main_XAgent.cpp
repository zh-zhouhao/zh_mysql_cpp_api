#include <iostream>
#include <string>
#include "XAgent.h"
#include "XCenter.h"


using namespace std;

//agent
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "please set ip" << endl;
        cout << "./agent 192.168.130.2(mysql_ip)" << endl;
        return false;
    }  
    // 1 初始化agent 连接数据库
    if (!XAgent::Get()->Init(argv[1]))
    {
        cout << "Agent init failed!" << endl;
        return -1;
    }
    XAgent::Get()->Main();
    cout << "Agent start!" << endl;
}