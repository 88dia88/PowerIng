#pragma once

#include "Common.h"
#include "Packet.h"


#define BUFSIZE 1024


using namespace std;


class GameClient
{
public:
    GameClient();
    ~GameClient();

    int Init();
    void ReleseSock();
    bool Connect();
    void SetServer(const string& IP, const int port);
    bool IsOnline();

    int SendPacket(const PacketType packetType, const Power_Player player);
    int RecvPacket(Power_Player& player);

    void SetClientID(int id);
    int GetClientID();

private:
    string mServerIP = "127.0.0.1";
    int mServerPort = 9000;

    WSADATA mWsa;
    SOCKET mSock;

    // clientData
    int mClientID;
    bool mOnline;

    // gameData

};