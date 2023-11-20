#pragma once
#include <string>
// ���� ������ ��ſ� ����� ��Ŷ ����

// ��Ŷ Ÿ��
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

// ������� Ű �Է��� ������ ���� ��Ŷ
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
