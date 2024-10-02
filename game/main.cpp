#include <cstdio>
#include "GameChannel.h"
#include "GameMsg.h"
#include "msg.pb.h"
#include "AOIWorld.h"
#include "ZinxTimer.h"
#include "RandomName.h"
#include <iostream>
using namespace std;

extern RandomName random_name;


void daemonlize()
{
	//1 fork
	int ipid = fork();
	if (0 > ipid)
	{
		exit(-1);
	}
	if (0 < ipid)
	{
		//2 父进程退出
		exit(0);
	}

	//3 子进程 设置回话ID
	setsid();
	//4 子进程 设置执行路径

	//5 子进程 重定向3个文件描述到/dev/null
	int nullfd = open("/dev/null", O_RDWR);
	if (nullfd >= 0)
	{
		dup2(nullfd, 0);
		dup2(nullfd, 1);
		dup2(nullfd, 2);
		close(nullfd);
	}
	//进程监控
	while (1)
	{
		int pid = fork();
		if (0 > pid) // 这里是pid小于0
		{
			exit(-1);
		}

		/*父进程等子进程退出*/
		if (0 < pid)//pid大于0，是父进程
		{
			int iStatus = 0;
			wait(&iStatus);
			if (0 == iStatus)
			{
				exit(0);
			}
		}
		/*子进程跳出循环执行游戏业务*/
		else // 否则就是子进程
		{
			break;
		}
	}

}



int main()
{
	daemonlize();
    random_name.LoadFile();

    ZinxKernel::ZinxKernelInit();//初始化
    /*添加监听通道*/
    ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(8899,new GameConnFact())));

    // 添加定时器
    ZinxKernel::Zinx_Add_Channel(*(new ZinxTimer()));

    ZinxKernel::Zinx_Run();
    ZinxKernel::ZinxKernelFini();
    return 0;
}