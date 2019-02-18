// TCPFighter_Server.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//


#include "pch.h"

#include "StreamQ.h"
#include "PacketBuffer.h"
#include "Protocol.h"
#include "User.h"

#define dfNETWORK_PORT 20000



SOCKET g_ListenSocket = INVALID_SOCKET;

UINT64 g_UserNo = 1;
unsigned int TPS = 0;
unsigned int CONNECT = 0;

FD_SET readSet;
FD_SET writeSet;

BOOL NetWork();
BOOL PacketProcess(User *pPacketUser, st_PACKET_HEADER packHeader);
void UserDisconnect(User* pClient);

std::list<User *> g_UserList;

int main()
{
	timeBeginPeriod(1);
	std::wcout.imbue(std::locale("korean"));

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cout << GetLastError() << std::endl;
		return -30;
	}

	SOCKADDR_IN serverAddr;

	g_ListenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (g_ListenSocket == INVALID_SOCKET) {
		std::cout << GetLastError() << std::endl;
		return -39;
	}

	memset(&serverAddr, 0, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(dfNETWORK_PORT);

	int retval = bind(g_ListenSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr));


	if (retval == SOCKET_ERROR) {
		std::cout << GetLastError() << std::endl;
		return -55;
	}

	retval = listen(g_ListenSocket, 0);

	if (retval == SOCKET_ERROR) {
		std::cout << GetLastError() << std::endl;
		return -62;
	}

	u_long on = 1;

	retval = ioctlsocket(g_ListenSocket, FIONBIO, &on);

	if (retval == SOCKET_ERROR) {
		std::cout << GetLastError() << std::endl;
		return -71;
	}

	TIMEVAL tVal;
	tVal.tv_sec = 0;
	tVal.tv_usec = 0;

	int fdSetCount = 0;

	std::cout << "Server Start -=-=-=-=-=-=-=-=-==-=-=-=-=-" << std::endl;

	while (1) {
		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);

		FD_SET(g_ListenSocket, &readSet);

		retval = select(0, &readSet, &writeSet, NULL, &tVal);

		if (retval == SOCKET_ERROR) {
			DWORD errorCode = GetLastError();
			if (errorCode != WSAEWOULDBLOCK) {
				std::cout << errorCode << std::endl;
				return -92;
			}
		}

		if (FD_ISSET(g_ListenSocket, &readSet)) {
			// connect 시킴

			User *newUser = new User;
			SOCKADDR_IN clientAddr;
			int addrLen = sizeof(clientAddr);

			newUser->m_socClient = accept(g_ListenSocket, (SOCKADDR *)&clientAddr, &addrLen);

			if (newUser->m_socClient == INVALID_SOCKET) {
				std::cout << " [ Line : " << __LINE__ << " ] ERROR NO :" << GetLastError() << std::endl;
				delete newUser;
			}
			else {
				char ipv4Addr[32] = { 0, };
				inet_ntop(clientAddr.sin_family, &clientAddr.sin_addr, ipv4Addr, sizeof(ipv4Addr));
				std::cout << "User Connect Ip : " << ipv4Addr << " port : " << ntohs(clientAddr.sin_port) << std::endl;
				g_UserList.push_back(newUser);
			}

		}


		FD_ZERO(&readSet);
		FD_ZERO(&writeSet);

		auto iter = g_UserList.begin();
		User* roopIter;
		for (std::list<User*>::iterator i = iter; i != g_UserList.end();) {
			roopIter = (*i);
			++i;

			FD_SET(roopIter->m_socClient, &readSet);
			FD_SET(roopIter->m_socClient, &writeSet);
			fdSetCount++;

			if (fdSetCount >= FD_SETSIZE) {
				retval = select(0, &readSet, &writeSet, NULL, &tVal);

				if (retval == SOCKET_ERROR) {
					DWORD errorCode = GetLastError();
					if (errorCode != WSAEWOULDBLOCK) {
						std::cout << errorCode << std::endl;
						return -127;
					}
				}

				if (retval > 0) {
					for (int j = 0; j < fdSetCount; ++j) {

						if (FD_ISSET((*iter)->m_socClient, &readSet)) {
							BOOL recvResult = (*iter)->NetRecvUser();

							if (!recvResult) {
								User* pClient = (*iter);
								++iter;
								UserDisconnect(pClient);

								std::cout << "recvResult fail disconnect" << std::endl;
								continue;
							}
						}

						if (FD_ISSET((*iter)->m_socClient, &writeSet)) {
							BOOL recvResult = (*iter)->NetSendUser();

							if (!recvResult) {
								User* pClient = (*iter);
								++iter;
								UserDisconnect(pClient);

								std::cout << "sendResult fail disconnect" << std::endl;
								continue;
							}
						}

						++iter;
					}
				}

				fdSetCount = 0;
				FD_ZERO(&readSet);
				FD_ZERO(&writeSet);
			}
		}

		retval = select(0, &readSet, &writeSet, NULL, &tVal);

		if (retval > 0) {

			for (int j = 0; j < fdSetCount && iter != g_UserList.end(); ++j) {

				if (FD_ISSET((*iter)->m_socClient, &readSet)) {
					BOOL recvResult = (*iter)->NetRecvUser();

					if (!recvResult) {
						User* pClient = (*iter);
						++iter;
						UserDisconnect(pClient);

						std::cout << "recvResult fail disconnect" << std::endl;
						continue;
					}
				}

				if (FD_ISSET((*iter)->m_socClient, &writeSet)) {
					BOOL sendResult = (*iter)->NetSendUser();

					if (!sendResult) {
						User* pClient = (*iter);
						++iter;
						UserDisconnect(pClient);

						std::cout << "sendResult fail disconnect" << std::endl;
						continue;
					}
				}

				++iter;
			}
		}

		fdSetCount = 0;

		NetWork();

		iter = g_UserList.begin();
	}

	closesocket(g_ListenSocket);
	WSACleanup();
	timeEndPeriod(1);

	return 0;
}

BOOL NetWork()
{
	for (auto iter = g_UserList.begin(); iter != g_UserList.end();) {
		auto user = (*iter);
		++iter;

		while (1) {
			int iRecvUseSize = user->m_pRecvQ->GetUseSize();

			if (iRecvUseSize < sizeof(st_PACKET_HEADER)) {
				break;
			}

			st_PACKET_HEADER packHeader;
			int retval = user->m_pRecvQ->Peek((char *)&packHeader, sizeof(st_PACKET_HEADER));

			// TODO 큐에 데이터 다 없으면 루프 탈출
			if (user->m_pRecvQ->GetUseSize() < packHeader.bySize + sizeof(st_PACKET_HEADER)) {
				break;
			}

			if (!PacketProcess(user, packHeader)) {
				// TODO 이상하게 처리됨 disconnect 함
				std::cout << " PACKET PROCESS FAIL " << std::endl;

				UserDisconnect(user);

				break;
			}
		}

	}

	return true;
}

BOOL PacketProcess(User * pPacketUser, st_PACKET_HEADER packHeader)
{
	PacketBuffer pack;
	WORD payloadSize = packHeader.bySize;
	st_PACKET_HEADER recvHeader;

	pPacketUser->m_pRecvQ->DeQueue((char *)&recvHeader, sizeof(recvHeader));
	pPacketUser->m_pRecvQ->DeQueue(pack.GetBufferPtr(), payloadSize);
	pack.MoveWritePos(payloadSize);

	switch (packHeader.byType) {
		//TODO 패킷 메세지 처리부
	case df_REQ_STRESS_ECHO:
		return NetPacket_ReqStressEcho(pPacketUser, &pack);
		break;

	default:
		return FALSE;
	}
}


void UserDisconnect(User * pClient)
{
	g_UserList.remove(pClient);

	closesocket(pClient->m_socClient);

	delete pClient;
}
