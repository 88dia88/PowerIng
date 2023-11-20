#pragma once
// ���� ������ ��ſ� ����� ��Ŷ ����

// ��Ŷ Ÿ��
enum PacketType
{
    PACKET_TYPE_DEFALT,
    PACKET_TYPE_KEY_INPUT,
    Count
};

#pragma pack(1)

// ������� Ű �Է��� ������ ���� ��Ŷ
struct KeyInputPacket
{
    const PacketType type = PACKET_TYPE_KEY_INPUT;

    bool up         = false;
    bool down       = false;
    bool right      = false;
    bool left       = false;
    bool action     = false;
};

#pragma pack()