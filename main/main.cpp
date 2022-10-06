#include <iostream>
#include <string>
#include "XAgent.h"
#include "XCenter.h"
#include "XClient.h"

using namespace std;

void Usage()
{
	cout << "========== Center Usage ==========" << endl;
	cout << "./main install 192.168.130.2" << endl;
	cout << "./main add 192.168.130.2 fileserver1" << endl;
}

int main(int argc, char *argv[])
{
	string ip = "127.0.0.1";
	if (argc > 1)
	{
		ip = argv[1]; // 
	}
	std::cout << "Client start!\n";
	if (!XClient::Get()->Init(ip))
	{
		return 0;
	}

	//初始化成功之后开始循环
	XClient::Get()->Main();

	return 0;
}
