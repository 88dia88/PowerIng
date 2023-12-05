#pragma once
#include <string>
// 게임 데이터 통신에 사용할 패킷 정의

const int MAX_NUM_CLIENTS = 3;
const int MAX_NUM_ORBS = 10;

#pragma pack(1)
// 패킷 타입
enum PacketType
{
    PACKET_TYPE_DEFALT,
    PACKET_TYPE_KEY_INPUT,
    PACKET_TYPE_LOBBY,
    PACKET_TYPE_IN_GAME,
    PACKET_TYPE_PLAYERS_DATA,
    PACKET_TYPE_CLIENT_DATA,
    PACKET_TYPE_CHANGE_GAME_STATE
};

enum ModuleType
{
    MODULE_TYPE_MODULE_NAME0,
    MODULE_TYPE_MODULE_NAME1,
    MODULE_TYPE_MODULE_NAME2,
    MODULE_TYPE_MODULE_NAME3,
    MODULE_TYPE_MODULE_NAME4
};

enum GameState
{
    GAME_STATE_LOBBY,
    GAME_STATE_READY,
    GAME_STATE_GAME,
    GAME_STATE_END,
};

struct KeyInput
{
    short up = 0x0000;
    short down = 0x0000;
    short right = 0x0000;
    short left = 0x0000;
    bool action = false;
};

struct Power_Orb // 오브 구조체
{
    bool major;
    double x, y;
    double speed, speedx, speedy, shellx, shelly;
    int RGB;
    int type, effect, effect_count;
    double angle, power, size;
    double afterx[25], aftery[25];
    struct Power_Orb* next;
};

struct Power_Reflector // 패널 구조체
{
    double polar_x, polar_y;
    int polar_speedx, polar_speedy;
    double size, speed;
    int RGB;
    int module[5], age;
    int Effect_effect, Effect_rebound;
    bool module_charged[5];
};

struct Power_Reactor // 리엑터 구조체 - 게임 상태 관리
{
    bool cherenkov, meltdown;
    int cherenkovlevel, meltdownlevel;
    int cherenkovmeter, cherenkovcounter;
};

struct Power_Player {
    bool Online = false, Ready = false;
    int RGB;
    int Score, CherenkovMeter;
    struct Power_Reflector Reflector;
    short upKeyDown = 0x0000;
    short downKeyDown = 0x0000;
    short rightKeyDown = 0x0000;
    short leftKeyDown = 0x0000;
    bool actionKeyDown = false;
};

struct Power_Team {
    int RGB;
    int ID[7];
    int life;
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

    int clientID;
    int playerCount;

    Power_Player players[MAX_NUM_CLIENTS];
};


struct PlayersDataPacket
{
    const PacketType type = PACKET_TYPE_PLAYERS_DATA;
    int clientNum = 0;

    Power_Player players[MAX_NUM_CLIENTS];
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
    Power_Player players[MAX_NUM_CLIENTS];

    Power_Reactor reactor;
    int orbCount;
    Power_Orb orbs[MAX_NUM_ORBS];
};

struct ChangeGameStatePacket
{
    const PacketType type = PACKET_TYPE_CHANGE_GAME_STATE;

    GameState gameState;
};



#pragma pack()