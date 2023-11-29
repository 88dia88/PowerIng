#pragma once
#include <string>
#include "Player.h"
// 게임 데이터 통신에 사용할 패킷 정의

// 패킷 타입
enum PacketType
{
    PACKET_TYPE_DEFALT,
    PACKET_TYPE_KEY_INPUT,
    PACKET_TYPE_LOBBY,
    PACKET_TYPE_IN_GAME,
    PACKET_TYPE_PLAYER_DATA,
    Count
};

enum ModuleType
{
    MODULE_TYPE_MODULE_NAME0,
    MODULE_TYPE_MODULE_NAME1,
    MODULE_TYPE_MODULE_NAME2,
    MODULE_TYPE_MODULE_NAME3,
    MODULE_TYPE_MODULE_NAME4,
    Count
};

#pragma pack(1)

struct KeyInput
{
    bool up = false;
    bool down = false;
    bool right = false;
    bool left = false;
    bool action = false;
};

// 사용자의 키 입력을 전송을 위한 패킷
struct KeyInputPacket
{
    const PacketType type = PACKET_TYPE_KEY_INPUT;

    KeyInput keyInput;
};

struct LobbyDataPacket
{
    const PacketType type = PACKET_TYPE_LOBBY;

    int playerCount;
    Player* players;
};

struct PlayerDataPacket
{
    const PacketType type = PACKET_TYPE_PLAYER_DATA;

    Player player;
};

struct BallData
{
    double x, y;
    int color;
    int type;
};

struct GameDataPacket
{
    const PacketType type = PACKET_TYPE_IN_GAME;

    int playerCount;
    Player* players;
    BallData ballData;
};



#pragma pack()