#include "GameRole.h"
#include "GameProtocol.h"
#include "GameChannel.h"
#include "GameMsg.h"
#include <iostream>
#include "AOIWorld.h"
#include "msg.pb.h"
#include <algorithm>
#include <random>
#include "ZinxTimer.h"
#include "RandomName.h"
#include <iostream>
#include <fstream>
#include <string>
#include <hiredis/hiredis.h>

using namespace std;

RandomName random_name;
static default_random_engine random_engine(time(NULL));
//������Ϸ����ȫ�ֶ���
static AOIWorld world(0, 400, 0, 400, 20, 20);

using namespace std;

GameMsg* GameRole::CreateIDNameLogin()
{
    pb::SyncPid* pmsg = new pb::SyncPid();
    pmsg->set_pid(iPid);
    pmsg->set_username(szName);
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);
    return pRet;
}

GameMsg* GameRole::CreataSrdPlayers()
{
    pb::SyncPlayers* pMsg = new pb::SyncPlayers();
    auto srd_list = world.GetSrdPlayer(this);
    for (auto single : srd_list) {
        auto pPlayer = pMsg->add_ps();
        auto pRole = dynamic_cast<GameRole*>(single);
        pPlayer->set_pid(pRole->iPid);
        pPlayer->set_username(pRole->szName);
        auto pPostion = pPlayer->mutable_p();
        pPostion->set_x(pRole->x);
        pPostion->set_y(pRole->y);
        pPostion->set_z(pRole->z);
        pPostion->set_v(pRole->v);
    }
    

    GameMsg* pret = new GameMsg(GameMsg::MSG_TYPE_SRD_POSTION, pMsg);
    return nullptr;
}

GameMsg* GameRole::CreateSelfPostion()
{
    pb::BroadCast* pMsg = new pb::BroadCast();
    pMsg->set_pid(iPid);
    pMsg->set_username(szName);
    pMsg->set_tp(2); //�ͻ��˾�����Ҫ���ó�2

    auto pPosition = pMsg->mutable_p();
    pPosition->set_x(x);
    pPosition->set_y(y);
    pPosition->set_z(z);
    pPosition->set_v(v);

    GameMsg* pret = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);
    return pret;
}

GameMsg* GameRole::CreateIDNameLogoff()
{
    pb::SyncPid* pmsg = new pb::SyncPid();
    pmsg->set_pid(iPid);
    pmsg->set_username(szName);
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);
    return pRet;
}

void GameRole::ProcTalkMsg(std::string _content)
{
    //����������
    auto role_list = ZinxKernel::Zinx_GetAllRole();
    for (auto pRole : role_list)
    {
        auto pGameRole = dynamic_cast<GameRole*>(pRole);
        auto pmsg = CreateTalkBroadCast(_content);
        ZinxKernel::Zinx_SendOut(*pmsg, *(pGameRole->m_pProto));
    }
}

void GameRole::ProcMoveMsg(float _x, float _y, float _z, float _v)
{
    /*1.��������*/
    /*��ȡԭ�����ھ�s1*/
    auto s1 = world.GetSrdPlayer(this);
    /*ժ���ɸ���*/
    world.DelPlayer(this);

    /*��������,����¸��ӣ���ȡ���ھ�s2*/
    x = _x;
    y = _y;
    z = _z;
    v = _v;
    world.AddPlayer(this);

    auto s2 = world.GetSrdPlayer(this);
    /*����s2����Ԫ�ز�����s1, ��Ұ����*/
    for (auto single_player : s2)
    {
        if (s1.end() == find(s1.begin(), s1.end(), single_player))
        {
            //��Ұ����
            ViewAppear(dynamic_cast<GameRole*>(single_player));
        }
    }

    /*����s1����Ԫ�ز�����s2����Ұ��ʧ*/
    for (auto single_player : s1)
    {
        if (s2.end() == find(s2.begin(), s2.end(), single_player))
        {
            //��Ұ��ʧ
            ViewLost(dynamic_cast<GameRole*>(single_player));
        }
    }

    /*2.�㲥��λ�ø���Χ���*/
    //������Χ��ҷ���
    /*����Χ��ҷ����Լ���λ��*/
    auto srd_list = world.GetSrdPlayer(this);
    for (auto single : srd_list)
    {
        //��ɴ����͵ı���
        pb::BroadCast* pMsg = new pb::BroadCast();
        auto pPos = pMsg->mutable_p();
        pPos->set_x(_x);
        pPos->set_y(_y);
        pPos->set_z(_z);
        pPos->set_v(_v);

        pMsg->set_pid(iPid);
        pMsg->set_tp(4);
        pMsg->set_username(szName);

        auto pRole = dynamic_cast<GameRole*>(single);

        //��װ����Ϣ����ȥ
        ZinxKernel::Zinx_SendOut(*(new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg)), *(pRole->m_pProto));
    }
}

void GameRole::ViewAppear(GameRole* _pRole)
{
    /*���Լ���������200��Ϣ*/
    auto pmsg = _pRole->CreateSelfPostion();
    ZinxKernel::Zinx_SendOut(*pmsg, *m_pProto);

    /*�������ҷ��Լ���200��Ϣ*/
    pmsg = CreateSelfPostion();
    ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->m_pProto));

}

void GameRole::ViewLost(GameRole* _pRole)
{
    /*���Լ����Ͳ�����ҵ�201��Ϣ*/
    auto pmsg = _pRole->CreateIDNameLogoff();
    ZinxKernel::Zinx_SendOut(*pmsg, *m_pProto);

    /*�������ҷ��Լ���201��Ϣ*/
    pmsg = CreateIDNameLogoff();
    ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->m_pProto));

}

GameMsg* GameRole::CreateTalkBroadCast(std::string _content)
{
    pb::BroadCast* pmsg = new pb::BroadCast();
    pmsg->set_pid(iPid);
    pmsg->set_username(szName);
    pmsg->set_tp(1);
    pmsg->set_content(_content);
    GameMsg* pRet = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pmsg);
    return pRet;
}

GameRole::GameRole()
{
    szName = random_name.GetName();
    x = 100 + random_engine() % 50;
    z = 100 + random_engine() % 50;
}

GameRole::~GameRole()
{
    random_name.Release(szName);
}

class ExitTimer :public TimerOutProc {
    // ͨ�� TimerOutProc �̳�
    virtual void Proc() override
    {
        ZinxKernel::Zinx_Exit();
    }
    virtual int GetTimerSec() override
    {
        return 20;
    }
};
static ExitTimer g_exit_timer;

bool GameRole::Init()
{
    if (ZinxKernel::Zinx_GetAllRole().size() <= 0)
    {
        TimerOutMng::GetInstance().DelTask(&g_exit_timer);
    }
    //����Լ�����Ϸ����
    bool bRet = false;
    //�������ID Ϊ��ǰ���ӵ�fd
    iPid = m_pProto->m_channel->GetFd();
    bRet = world.AddPlayer(this);

    //���Լ�����ID������
    if (true == bRet) {
        /*���Լ�����ID������*/
        auto pmsg = CreateIDNameLogin();
        ZinxKernel::Zinx_SendOut(*pmsg, *m_pProto);
        /*���Լ�������Χ��ҵ�λ��*/
        pmsg = CreataSrdPlayers();
        ZinxKernel::Zinx_SendOut(*pmsg, *m_pProto);
        /*����Χ��ҷ����Լ���λ��*/
        auto srd_list = world.GetSrdPlayer(this);
        for (auto single : srd_list)
        {
            pmsg = CreateSelfPostion();
            auto pRole = dynamic_cast<GameRole*>(single);

            // ע��ڶ�������
            ZinxKernel::Zinx_SendOut(*pmsg, *(pRole->m_pProto));
        }

    }

    //��¼��ǰ������redis��game_name
    //1 ����redis
    auto context = redisConnect("127.0.0.1", 6379);
    //2 ����lpush����
    if (NULL != context)
    {
        freeReplyObject(redisCommand(context, "lpush game_name %s", szName.c_str()));
        redisFree(context);
    }


    return bRet;
}

/*������Ϸ��ص��û�����*/
UserData* GameRole::ProcMsg(UserData& _poUserData)
{
    /*����*/
    GET_REF2DATA(MultiMsg, input, _poUserData);

    for (auto single : input.m_Msgs) {
        cout << "type" << single->enMsgType << endl;
        cout << single->pMsg->Utf8DebugString() << endl;

        auto NewPos = dynamic_cast<pb::Position*>(single->pMsg);

        switch (single->enMsgType)
        {
        case GameMsg::MSG_TYPE_CHAT_CONTENT:
            ProcTalkMsg(dynamic_cast<pb::Talk*>(single->pMsg)->content());
            break;
        case GameMsg::MSG_TYPE_NEW_POSTION:
            ProcMoveMsg(NewPos->x(), NewPos->y(), NewPos->z(), NewPos->v());
            break;
        default:
            break;
        }
    }


    return nullptr;
}

void GameRole::Fini()
{
    //����Χ��ҷ���������Ϣ
    auto srd_list = world.GetSrdPlayer(this);
    for (auto single : srd_list)
    {
        auto pMsg = CreateIDNameLogoff();
        auto pRole = dynamic_cast<GameRole*>(single);
        ZinxKernel::Zinx_SendOut(*pMsg, *(pRole->m_pProto));
    }
    world.DelPlayer(this);

    /*�ж��Ƿ������һ�����--->��ʱ��*/
    if (ZinxKernel::Zinx_GetAllRole().size() <= 1)
    {
        //���˳���ʱ��
        TimerOutMng::GetInstance().AddTask(&g_exit_timer);
    }

    //��redis  game_name��ɾ����ǰ����
    auto context = redisConnect("127.0.0.1", 6379);
    if (NULL != context)
    {
        freeReplyObject(redisCommand(context, "lrem game_name 1 %s", szName.c_str()));
        redisFree(context);
    }

}

int GameRole::GetX()
{
    return (int)x;
}

int GameRole::GetY()
{
    return (int)z;
}
