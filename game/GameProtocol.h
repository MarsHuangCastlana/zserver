#pragma once
#include <zinx.h>
#include <iostream>

using namespace std;
class GameChannel;
class GameRole;

class GameProtocol :
    public Iprotocol
{
    std::string szlast;
public:
    GameChannel* m_channel = NULL; 
    GameRole* m_Role = NULL;
    ~GameProtocol();
    // Í¨¹ý Iprotocol ¼Ì³Ð
    UserData* raw2request(std::string _szInput) override;
    std::string* response2raw(UserData& _oUserData) override;
    Irole* GetMsgProcessor(UserDataMsg& _oUserDataMsg) override;
    Ichannel* GetMsgSender(BytesMsg& _oBytes) override;
};

