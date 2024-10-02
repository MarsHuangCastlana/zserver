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

/*���������ͨ��������ԭʼ����
����ֵ��ת�������Ϣ����
ת����ʽ:tcpճ������*/
UserData* GameProtocol::raw2request(std::string _szInput)
{
    MultiMsg *pRet = new MultiMsg();
    szlast.append(_szInput);

    while (1) {
        if (szlast.size() < 8) {
            break;
        }

        /*��ǰ�ĸ��ֽڶ�ȡ��Ϣ���ݳ���*/
        int iLength = 0;
        iLength |= szlast[0] << 0;
        iLength |= szlast[1] << 8;
        iLength |= szlast[2] << 16;
        iLength |= szlast[3] << 24;
        /*�����ĸ��ֽڶ�ȡ��Ϣ���ݳ���*/
        int id = 0;
        id |= szlast[4] << 0;
        id |= szlast[5] << 8;
        id |= szlast[6] << 16;
        id |= szlast[7] << 24;

        /*ͨ�������ĳ̶��ж������ı����Ƿ�Ϸ�*/
        if (szlast.size() - 8 < iLength) {
            /*�������ĳ��Ȳ����������*/
            break;
        }

        /*����һ���û�����*/
        GameMsg* pMsg = new GameMsg((GameMsg::MSG_TYPE)id, szlast.substr(8, iLength));
        pRet->m_Msgs.push_back(pMsg);
        /*�����Ѿ�����ı���*/
        szlast.erase(0, 8 + iLength);
    }

    return pRet;
}

/*��������ҵ��㣬�����͵���Ϣ
����ֵת������ֽ���*/
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

/*�������ݷ��͵�ͨ��*/
Ichannel* GameProtocol::GetMsgSender(BytesMsg& _oBytes)
{

    return m_channel;
}
