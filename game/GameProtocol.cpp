#include "GameProtocol.h"
#include "GameMsg.h"
#include "GameChannel.h"
#include "GameRole.h"
#include "msg.pb.h"
#include <iostream>

using namespace std;

GameProtocol::~GameProtocol()
{
    if (NULL != m_Role) {
        ZinxKernel::Zinx_Del_Role(*m_Role);
        delete m_Role;
    }
}

/*输入参数是通道传来的原始报文
返回值是转换后的消息对象
转换方式:tcp粘包处理*/
UserData* GameProtocol::raw2request(std::string _szInput)
{
    MultiMsg *pRet = new MultiMsg();
    szlast.append(_szInput);

    while (1) {
        if (szlast.size() < 8) {
            break;
        }

        /*在前四个字节读取消息内容长度*/
        int iLength = 0;
        iLength |= szlast[0] << 0;
        iLength |= szlast[1] << 8;
        iLength |= szlast[2] << 16;
        iLength |= szlast[3] << 24;
        /*在中四个字节读取消息内容长度*/
        int id = 0;
        id |= szlast[4] << 0;
        id |= szlast[5] << 8;
        id |= szlast[6] << 16;
        id |= szlast[7] << 24;

        /*通过读到的程度判定后续的报文是否合法*/
        if (szlast.size() - 8 < iLength) {
            /*本条报文长度不够，不理会*/
            break;
        }

        /*构造一条用户请求*/
        GameMsg* pMsg = new GameMsg((GameMsg::MSG_TYPE)id, szlast.substr(8, iLength));
        pRet->m_Msgs.push_back(pMsg);
        /*弹出已经处理的报文*/
        szlast.erase(0, 8 + iLength);
    }

    return pRet;
}

/*参数来自业务层，待发送的消息
返回值转换后的字节流*/
std::string* GameProtocol::response2raw(UserData& _oUserData)
{
    int iLength = 0;
    int id = 0;
    std::string MsgContent;
    auto pret = new std::string();

    GET_REF2DATA(GameMsg, oOutput, _oUserData);
    id = oOutput.enMsgType;
    MsgContent = oOutput.serialize();
    iLength = MsgContent.size();

    pret->push_back((iLength >> 0) & 0xff);
    pret->push_back((iLength >> 8) & 0xff);
    pret->push_back((iLength >> 16) & 0xff);
    pret->push_back((iLength >> 24) & 0xff);
    pret->push_back((id >> 0) & 0xff);
    pret->push_back((id >> 8) & 0xff);
    pret->push_back((id >> 16) & 0xff);
    pret->push_back((id >> 24) & 0xff);


    pret->append(MsgContent);

    return pret;
}

Irole* GameProtocol::GetMsgProcessor(UserDataMsg& _oUserDataMsg)
{
    return m_Role;
}

/*返回数据发送的通道*/
Ichannel* GameProtocol::GetMsgSender(BytesMsg& _oBytes)
{

    return m_channel;
}
