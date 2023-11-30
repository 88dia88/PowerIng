#include "Client.h"
//
//int RecvS2CPacket(SOCKET clientSock, int clientID)
//{
//	char buf[BUFSIZE + 1];
//	int retval = recv(clientSock, buf, BUFSIZE, 0);
//
//	if (retval == SOCKET_ERROR || retval == 0) {
//		return retval;
//	}
//	else if (retval < sizeof(PacketType)) {
//		printf("error ��Ŷ ����� �ʹ� �۽��ϴ�. %s\n", buf);
//		return retval;
//	}
//
//	PacketType type;
//	memcpy(&type, buf, sizeof(PacketType));
//
//	switch (type) {
//	case PACKET_TYPE_KEY_INPUT: {
//		if (retval < sizeof(KeyInputPacket)) {
//			printf("Error: ��Ŷ ������ ���� KeyInputPacket.\n");
//			return retval;
//		}
//
//		KeyInputPacket keyInputPacket;
//		memcpy(&keyInputPacket, buf, sizeof(KeyInputPacket));
//
//		players[clientID].SetKeyInput(keyInputPacket.keyInput);
//		printf("%d client, up=%s, down=%s, right=%s, left=%s, action=%s\n",
//			clientID,
//			keyInputPacket.keyInput.up ? "true" : "false",
//			keyInputPacket.keyInput.down ? "true" : "false",
//			keyInputPacket.keyInput.right ? "true" : "false",
//			keyInputPacket.keyInput.left ? "true" : "false",
//			keyInputPacket.keyInput.action ? "true" : "false");
//		break;
//	}
//	case PACKET_TYPE_PLAYERS_DATA: {
//		if (retval < sizeof(PlayersDataPacket)) {
//			printf("Error: ��Ŷ ������ ���� PlayerDataPacket.\n");
//			return retval;
//		}
//
//		PlayersDataPacket playerDataPacket;
//		memcpy(&playerDataPacket, buf, sizeof(PlayersDataPacket));
//
//		players[clientID].SetPlayerData(playerDataPacket.player);
//		break;
//	}
//	case PACKET_TYPE_CLIENT_DATA: {
//		if (retval < sizeof(ClientDataPacket)) {
//			printf("Error: ��Ŷ ������ ���� ClientDataPacket.\n");
//			return retval;
//		}
//
//		ClientDataPacket clientDataPacket;
//		memcpy(&clientDataPacket, buf, sizeof(ClientDataPacket));
//
//		players[clientID].SetPanel(clientDataPacket.color, clientDataPacket.module);
//		printf("clientID: %d - clientColor: %d, clientModule: %d\n",
//			clientID, clientDataPacket.color, clientDataPacket.module);
//		break;
//	}
//	default:
//		printf("error ��ŶŸ���� ���ǵ��� �ʾҽ��ϴ�.\n");
//		break;
//	}
//
//	return retval;
//
//}
//
//int SendC2SPacket(SOCKET clientSock, int clientID)
//{
//	int retval = 0;
//
//	switch (gameState)
//	{
//
//	// InGame
//	case GAME_STATE_GAME: {
//		GameDataPacket gameDataPacket;
//
//		// GameDataPacket ä���
//		gameDataPacket.playerCount = clientCnt;
//		//gameDataPacket.players = new Player[clientCnt];
//		//memcpy(gameDataPacket.players, players, sizeof(Player) * clientCnt);
//		// gameDataPacket.ballData = ballData;
//
//		// ������ ����
//		send(clientSock, reinterpret_cast<char*>(&gameDataPacket), sizeof(GameDataPacket), 0);
//
//		//delete[] gameDataPacket.players;
//		return retval;
//		break;
//	}
//	
//	// InLobby
//	case GAME_STATE_LOBBY: {
//		LobbyDataPacket lobbyDataPacket;
//
//		// GameDataPacket ä���
//		lobbyDataPacket.playerCount = clientCnt;
//		lobbyDataPacket.clientID = clientID;
//
//		//lobbyDataPacket.players = new Player[clientCnt];
//		//memcpy(lobbyDataPacket.players, players, sizeof(Player) * clientCnt);
//
//		// ������ ����
//		send(clientSock, reinterpret_cast<char*>(&lobbyDataPacket), sizeof(LobbyDataPacket), 0);
//
//		//delete[] lobbyDataPacket.players;
//		return retval;
//	}
//
//						 // ReadyRound
//	case GAME_STATE_READY:
//
//		return retval;
//
//	default:
//		return 0;
//	}
//}