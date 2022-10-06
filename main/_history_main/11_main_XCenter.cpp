#include <iostream>
#include <string>
#include "XAgent.h"
#include "XCenter.h"


using namespace std;

void Usage()
{
	cout << "========== Center Usage ==========" << endl;
	cout << "./main install 192.168.130.2" << endl;
    cout << "./main add 192.168.130.2 fileserver1" << endl;
}


int main(int argc, char *argv[])
{
    string cmd;
	if (argc > 1)
	{
		cmd = argv[1];
	}

    // 安装系统
	if (cmd == "install")
	{
		cout << "begin install center" << endl;
		if (argc < 3) //至少要三个参数
		{
			Usage();
			return 0;
		}
		XCenter::Get()->Install(argv[2]);//安装
        return 0;
	}

    if (!XCenter::Get()->Init())
	{
		cerr << "XCenter::Get()->Init() failed!" << endl;
		return -1;
	}

    if(cmd == "add")
    {
		cout << "add device" << endl;
		//./main add 192.168.130.2 fileserver1
		if (argc < 4)
		{
			Usage();
			return 0;
		}
		XCenter::Get()->AddDevice(argv[2], argv[3]);//添加
        return 0;
    }

    cout << "Center start!\n";

    XCenter::Get()->Main(); 
  

    return 0;
}