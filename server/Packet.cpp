#include "Packit.h"

void ProcessPacket(const char* buffer, int bufferSize)
{
    if (bufferSize < sizeof(PacketType)) {
        printf("error ��Ŷ ����� �ʹ� �۽��ϴ�. %s\n", buffer);
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

        // ó�� ����
        printf("Ű ��ǲ ó��, up=%s, down=%s, right=%s, left=%s, action=%s",
            keyInputPacket.up ? "true" : "false",
            keyInputPacket.down ? "true" : "false",
            keyInputPacket.right ? "true" : "false",
            keyInputPacket.left ? "true" : "false",
            keyInputPacket.action ? "true" : "false");

        break;
    }
                              // �ٸ� ��Ŷ Ÿ�Ե� ����ó��
    default:
        printf("error ��ŶŸ���� ���ǵ��� �ʾҽ��ϴ�.\n");
        break;
    }
}