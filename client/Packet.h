#pragma once
#include <string>
// ���� ������ ��ſ� ����� ��Ŷ ����

const int MAX_NUM_CLIENTS = 3;
const int MAX_NUM_ORBS = 10;

#pragma pack(1)
// ��Ŷ Ÿ��
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

struct Power_Orb // ���� ����ü
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

struct Power_Reflector // �г� ����ü
{
    double polar_x = 0, polar_y = 375;
    int polar_speedx = 0, polar_speedy = 0;
    double size = 375, speed = 1;
    int age = 0, cherenkovcounter = 0;
    int RGB = 0xffff00;
    int module[5] = { 0, 0, 0, 0, 0 };
    bool module_charged[5] = { false, false, false, false, false };
    int Effect_effect = 0, Effect_rebound = 0;
};

struct Power_Reactor // ������ ����ü - ���� ���� ����
{
    bool cherenkov, meltdown;
    int cherenkovlevel, meltdownlevel;
    int cherenkovmeter, cherenkovcounter;
};

struct Power_Statistic {
    double TotalScore = 0, HighestScore = 0;
    int ReflectCount = 0, BonusCount = 0, CherenkovCount = 0;
    double ReflectSpeed = 0, MoveDistance = 0;
};

struct Power_Player {
    bool Online = false, Ready = false;
    int ID = 0;
    int RGB = 0xffff00;
    int CherenkovMeter = 0, Count = 3;
    double Score = 0;
    short Control[5] = { 0, 0, 0, 0, 0 };
    struct Power_Reflector Reflector;
    struct Power_Statistic Statistic;
};

struct Power_Team {
    int RGB;
    int ID[7];
    int life;
};

// ������� Ű �Է��� ������ ���� ��Ŷ
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