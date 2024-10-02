#pragma once
#include <zinx.h>
#include "AOIWorld.h"
#include "GameMsg.h"

class GameProtocol; 

class GameRole :
    public Irole,public Player
{
    float x = 0;
    float y = 0;//高
    float z = 0;
    float v = 0;
    int iPid = 0;
    std::string szName;
    GameMsg* CreateIDNameLogin();
    GameMsg* CreataSrdPlayers();
    GameMsg* CreateSelfPostion();
    GameMsg* CreateIDNameLogoff();
    void ProcTalkMsg(std::string _content);
    void ProcMoveMsg(float _x, float _y, float _z, float _v);

    void ViewAppear(GameRole* _pRole);
    void ViewLost(GameRole* _pRole);
    GameMsg* CreateTalkBroadCast(std::string _content);
public:
    GameRole();
    ~GameRole();
    // 通过 Irole 继承
    bool Init() override;
    UserData* ProcMsg(UserData& _poUserData) override;
    void Fini() override;
    GameProtocol* m_pProto = NULL;

    // 通过 Player 继承
    int GetX() override;
    int GetY() override;
};

