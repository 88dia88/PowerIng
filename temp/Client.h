#include "../Globals.h"
#include "../GameTimer.h"
#include "../Packit.h"
#include "../GamePlayer.h"

#define SERVERPORT 9000
#define BUFSIZE    1024
#define SERVERIP "127.0.0.1"

int RecvS2CPacket(SOCKET clientSock, int clientID);
int SendC2SPacket(SOCKET clientSock, int clientID);

//int gameState;
//bool allReady = true;
//GameTimer gameTimer;
//LONG clientCnt = 0;
//CRITICAL_SECTION cs;
//GamePlayer players[MAX_NUM_CLIENTS];
