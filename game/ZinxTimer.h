//#pragma once
#include <zinx.h>
#include <list>
#include <vector>

class ZinxTimer :
    public Ichannel
{
    int m_TimerFd = -1;
public:
    // ͨ�� Ichannel �̳�
    bool Init() override;
    bool ReadFd(std::string& _input) override;
    bool WriteFd(std::string& _output) override;
    void Fini() override;
    int GetFd() override;
    std::string GetChannelInfo() override;
    AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;
};




class TimerOutProc {
public:
    virtual void Proc() = 0;
    virtual int GetTimerSec() = 0;
    /*��ʣȦ��*/
    int iCount = -1;
};


class TimerOutMng:public AZinxHandler
{
    std::vector<std::list<TimerOutProc*>> m_timer_wheel;
    int cur_index = 0;
    static TimerOutMng single;
    TimerOutMng();
public:
    
    // ͨ�� AZinxHandler �̳�
    /*����ʱ�¼����������г�ʱ�¼�*/
    IZinxMsg* InternelHandle(IZinxMsg& _oInput) override;

    AZinxHandler* GetNextHandler(IZinxMsg& _oNextMsg) override;

    void AddTask(TimerOutProc* _ptask);
    void DelTask(TimerOutProc* _ptask);

    static TimerOutMng &GetInstance() {
        return single;
    }
};


