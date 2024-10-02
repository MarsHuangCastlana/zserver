#pragma once
#include <ZinxTCP.h>
#include "GameProtocol.h"


class GameChannel :
    public ZinxTcpData
{
public:
    GameChannel(int _fd);
    ~GameChannel();


    // 通过 ZinxTcpData 继承
    GameProtocol* m_proto = NULL;
    /*返回协议对象*/
    AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;

    
};


class GameConnFact:public IZinxTcpConnFact
{
public:

    // 通过 IZinxTcpConnFact 继承
    ZinxTcpData* CreateTcpDataChannel(int _fd) override;
};




