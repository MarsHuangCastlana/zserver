#include "ZinxTimer.h"
#include <sys/timerfd.h>
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;


/*创建定时器文件描述符*/
bool ZinxTimer::Init()
{
    bool bRet = false;
    /*创建文件描述符*/
    int iFd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (iFd >= 0) {
        /*设置定时周期*/
        struct itimerspec period=
        {
            {1,0},
            {1,0}
        };
        if (timerfd_settime(iFd,0,&period,NULL) == 0) {
            bRet = true;
            m_TimerFd = iFd;
        }

    }
    
    return bRet;
}

/*读取超时次数*/
bool ZinxTimer::ReadFd(std::string& _input)
{
    bool bRet = false;
    char buff[8] = { 0 };
    if (read(m_TimerFd, buff, sizeof(buff)) == sizeof(buff)) {
        bRet = true;
        _input.assign(buff, sizeof(buff));
    }
    return bRet;
}

bool ZinxTimer::WriteFd(std::string& _output)
{
    return false;
}

/*关闭文件描述符*/
void ZinxTimer::Fini()
{
    close(m_TimerFd);
    m_TimerFd = -1;
}


/*返回当前定时器文件描述符*/
int ZinxTimer::GetFd()
{
    return m_TimerFd;
}

std::string ZinxTimer::GetChannelInfo()
{
    return "TimerFd";
}

class output_hello :public AZinxHandler
{
public:

    // 通过 AZinxHandler 继承
    IZinxMsg* InternelHandle(IZinxMsg& _oInput) override
    {
        auto pchannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
        string output = "hello word";
        ZinxKernel::Zinx_SendOut(output, *pchannel);
        return nullptr;
    }

    AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override
    {
        return nullptr;
    }

}*poutput_hello=new output_hello();


/*返回处理超时时间的对象*/
AZinxHandler* ZinxTimer::GetInputNextStage(BytesMsg& _oInput)
{
    return &TimerOutMng::GetInstance();
}

TimerOutMng TimerOutMng::single;

TimerOutMng::TimerOutMng()
{
    for (int i = 0; i < 10; ++i) {
        list<TimerOutProc*> tmp;
        m_timer_wheel.push_back(tmp);
    }
}

IZinxMsg* TimerOutMng::InternelHandle(IZinxMsg& _oInput)
{
    unsigned long iTimeoutCount = 0;

    GET_REF2DATA(BytesMsg, obytes, _oInput);
    obytes.szData.copy((char*) & iTimeoutCount, sizeof(iTimeoutCount), 0);


    while (iTimeoutCount-- > 0) {
        /*移动刻度*/
        cur_index++;
        cur_index %= 10;

        list<TimerOutProc*> m_cache;
        /*遍历当前刻度所有节点，指向处理函数或圈数减一*/
        for (auto itr = m_timer_wheel[cur_index].begin(); itr != m_timer_wheel[cur_index].end(); itr++)
        {
            /*如果圈数小于等于0————》执行超时函数  否则圈数-1*/
            if ((*itr)->iCount <= 0) {
                /*先缓存待处理的超时节点*/
                m_cache.push_back(*itr);
                /*将task摘出*/
                auto ptmp = *itr;
                itr = m_timer_wheel[cur_index].erase(itr);
                AddTask(ptmp);
            }
            else
            {
                (*itr)->iCount--;
                ++itr;
            }
        }


        /*统一执行待处理的超时任务*/
        for (auto task : m_cache) {
            task->Proc();
        }
    }
       
    return nullptr;
}



AZinxHandler* TimerOutMng::GetNextHandler(IZinxMsg& _oNextMsg)
{
    return nullptr;
}

void TimerOutMng::AddTask(TimerOutProc* _ptask)
{
    /*计算当前任务需要放到那个齿上*/
    int index = (_ptask->GetTimerSec() + cur_index) % 10;
    /*把任务存到该齿轮上*/
    m_timer_wheel[index].push_back(_ptask);
    /*计算所需圈数*/
    _ptask->iCount = _ptask->GetTimerSec() / 10;
}

void TimerOutMng::DelTask(TimerOutProc* _ptask)
{
     /*遍历时间轮所有齿，删掉任务*/
    for (list<TimerOutProc*> &chi : m_timer_wheel) {
        for (auto task : chi) {
            if (task == _ptask) {
                chi.remove(_ptask);
                return;
            }
        }
    }

    //bool found = false;
    //for (auto& chi : m_timer_wheel) { // 使用 auto& 避免不必要的拷贝  
    //    auto it = find(chi.begin(), chi.end(), _ptask);
    //    if (it != chi.end()) {
    //        chi.erase(it); // 使用迭代器删除元素  
    //        found = true;
    //        break; // 找到并删除后退出内层循环  
    //    }
    //}
}


 