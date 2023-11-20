#pragma once
#include <string>
// 게임 데이터 통신에 사용할 패킷 정의

// 패킷 타입
enum PacketType
{
    PACKET_TYPE_DEFALT,
    PACKET_TYPE_KEY_INPUT,
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

// 사용자의 키 입력을 전송을 위한 패킷
struct KeyInputPacket
{
    const PacketType type = PACKET_TYPE_KEY_INPUT;

    bool up = false;
    bool down = false;
    bool right = false;
    bool left = false;
    bool action = false;
};

struct LobbyPlayerPacket
{
    int id;
    int color;
    int module;
    bool playerReady;
    bool playerRule;
};

struct BallData 
{
    double x, y;
    int color;
    int type;
};

#pragma pack()

void ProcessPacket(const char* buffer, int bufferSize);
