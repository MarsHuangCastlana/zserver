#pragma once
#include <ZinxTCP.h>
#include "GameProtocol.h"


class GameChannel :
    public ZinxTcpData
{
public:
    GameChannel(int _fd);
    ~GameChannel();


    // ͨ�� ZinxTcpData �̳�
    GameProtocol* m_proto = NULL;
    /*����Э�����*/
    AZinxHandler* GetInputNextStage(BytesMsg& _oInput) override;

    
};


class GameConnFact:public IZinxTcpConnFact
{
public:

    // ͨ�� IZinxTcpConnFact �̳�
    ZinxTcpData* CreateTcpDataChannel(int _fd) override;
};




