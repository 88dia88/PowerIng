#pragma once
#include <string>
#include "GamePlayer.h"
// 게임 데이터 통신에 사용할 패킷 정의

// 패킷 타입
enum PacketType
{
    PACKET_TYPE_DEFALT,
    PACKET_TYPE_KEY_INPUT,
    PACKET_TYPE_LOBBY,
    PACKET_TYPE_IN_GAME,
    PACKET_TYPE_PLAYERS_DATA,
    PACKET_TYPE_CLIENT_DATA
};

enum ModuleType
{
    MODULE_TYPE_MODULE_NAME0,
    MODULE_TYPE_MODULE_NAME1,
    MODULE_TYPE_MODULE_NAME2,
    MODULE_TYPE_MODULE_NAME3,
    MODULE_TYPE_MODULE_NAME4
};

#pragma pack(1)

// 사용자의 키 입력을 전송을 위한 패킷
struct KeyInputPacket
{
    const PacketType type = PACKET_TYPE_KEY_INPUT;

    KeyInput keyInput;
};

struct LobbyDataPacket
{
    const PacketType type = PACKET_TYPE_LOBBY;

    int clientID;
    int playerCount;
    GamePlayer* players;
};

struct PlayersDataPacket
{
    const PacketType type = PACKET_TYPE_PLAYERS_DATA;

    GamePlayer player;
};

struct ClientDataPacket
{
    const PacketType type = PACKET_TYPE_CLIENT_DATA;

    int color;
    int module[5];
};


struct GameDataPacket
{
    const PacketType type = PACKET_TYPE_IN_GAME;

    int playerCount;
};



#pragma pack()