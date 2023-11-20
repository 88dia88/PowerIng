#include "Packit.h"

void ProcessPacket(const char* buffer, int bufferSize)
{
    if (bufferSize < sizeof(PacketType)) {
        printf("error 패킷 사이즈가 너무 작습니다. %s\n", buffer);
        return;
    }

    PacketType type;
    memcpy(&type, buffer, sizeof(PacketType));

    switch (type) {
    case PACKET_TYPE_KEY_INPUT: {
        if (bufferSize < sizeof(KeyInputPacket)) {
            return;
        }

        KeyInputPacket keyInputPacket;
        memcpy(&keyInputPacket, buffer, sizeof(KeyInputPacket));

        // 처리 로직
        printf("키 인풋 처리, up=%s, down=%s, right=%s, left=%s, action=%s",
            keyInputPacket.up ? "true" : "false",
            keyInputPacket.down ? "true" : "false",
            keyInputPacket.right ? "true" : "false",
            keyInputPacket.left ? "true" : "false",
            keyInputPacket.action ? "true" : "false");

        break;
    }
                              // 다른 패킷 타입들 예외처리
    default:
        printf("error 패킷타입이 정의되지 않았습니다.\n");
        break;
    }
}