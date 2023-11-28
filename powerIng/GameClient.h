#pragma once

#include "Common.h"
#include "Packet.h"
#include "GamePlayer.h"


#define BUFSIZE 1024


using namespace std;


struct Power_Player;


class GameClient
{
public:
    GameClient();
    ~GameClient();

    int Init();
    void ReleseSock();
    bool Connect();
    void SetServer(const string& IP, const int port);

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

    // gameData

};