#include "ZinxTimer.h"
#include <sys/timerfd.h>
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;


/*������ʱ���ļ�������*/
bool ZinxTimer::Init()
{
    bool bRet = false;
    /*�����ļ�������*/
    int iFd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (iFd >= 0) {
        /*���ö�ʱ����*/
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

/*��ȡ��ʱ����*/
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

/*�ر��ļ�������*/
void ZinxTimer::Fini()
{
    close(m_TimerFd);
    m_TimerFd = -1;
}


/*���ص�ǰ��ʱ���ļ�������*/
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

    // ͨ�� AZinxHandler �̳�
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


/*���ش���ʱʱ��Ķ���*/
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
        /*�ƶ��̶�*/
        cur_index++;
        cur_index %= 10;

        list<TimerOutProc*> m_cache;
        /*������ǰ�̶����нڵ㣬ָ��������Ȧ����һ*/
        for (auto itr = m_timer_wheel[cur_index].begin(); itr != m_timer_wheel[cur_index].end(); itr++)
        {
            /*���Ȧ��С�ڵ���0����������ִ�г�ʱ����  ����Ȧ��-1*/
            if ((*itr)->iCount <= 0) {
                /*�Ȼ��������ĳ�ʱ�ڵ�*/
                m_cache.push_back(*itr);
                /*��taskժ��*/
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


        /*ͳһִ�д�����ĳ�ʱ����*/
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
    /*���㵱ǰ������Ҫ�ŵ��Ǹ�����*/
    int index = (_ptask->GetTimerSec() + cur_index) % 10;
    /*������浽�ó�����*/
    m_timer_wheel[index].push_back(_ptask);
    /*��������Ȧ��*/
    _ptask->iCount = _ptask->GetTimerSec() / 10;
}

void TimerOutMng::DelTask(TimerOutProc* _ptask)
{
     /*����ʱ�������гݣ�ɾ������*/
    for (list<TimerOutProc*> &chi : m_timer_wheel) {
        for (auto task : chi) {
            if (task == _ptask) {
                chi.remove(_ptask);
                return;
            }
        }
    }

    //bool found = false;
    //for (auto& chi : m_timer_wheel) { // ʹ�� auto& ���ⲻ��Ҫ�Ŀ���  
    //    auto it = find(chi.begin(), chi.end(), _ptask);
    //    if (it != chi.end()) {
    //        chi.erase(it); // ʹ�õ�����ɾ��Ԫ��  
    //        found = true;
    //        break; // �ҵ���ɾ�����˳��ڲ�ѭ��  
    //    }
    //}
}


 