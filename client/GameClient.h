#include "Common.h"
#include "Packit.h"
#include "Player.h"
#include <string>


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

    int SendPacket(const PacketType packetType);
    int RecvPacket();

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