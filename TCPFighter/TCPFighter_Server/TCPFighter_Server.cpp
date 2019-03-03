// TCPFighter_Server.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//


#include "pch.h"

#include "StreamQ.h"
#include "PacketBuffer.h"
#include "Protocol.h"
#include "User.h"
#include "Network.h"
#include "Sector.h"

SOCKET g_ListenSocket = INVALID_SOCKET;

DWORD g_UID = 1;
unsigned int TPS = 0;
unsigned int CONNECT = 0;
BOOL g_bShutdown = false;
DWORD g_GameFrameTick = 0;
DWORD g_GameMonitorTick = 0;
int g_FrameCount = 0;
int g_LoopCount = 0;



std::list<User *> g_UserList;
std::list<User *> g_Sector[dfSECTOR_MAX_Y][dfSECTOR_MAX_X];


BOOL NetWork();
BOOL PacketProcess(User *pPacketUser, st_PACKET_HEADER packHeader);
void UserDisconnect(User* pClient);
void SocketSet();
void Monitor();
void Update();
BOOL CharacterMoveCheck(int x, int y);


int main()
{
	timeBeginPeriod(1);
	std::wcout.imbue(std::locale("korean"));

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		std::cout << GetLastError() << std::endl;
		return -40;
	}

	SOCKADDR_IN serverAddr;

	g_ListenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (g_ListenSocket == INVALID_SOCKET) {
		std::cout << GetLastError() << std::endl;
		return -49;
	}

	memset(&serverAddr, 0, sizeof(serverAddr));

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(dfPORT);

	int retval = bind(g_ListenSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr));


	if (retval == SOCKET_ERROR) {
		std::cout << GetLastError() << std::endl;
		return -63;
	}

	retval = listen(g_ListenSocket, 0);

	if (retval == SOCKET_ERROR) {
		std::cout << GetLastError() << std::endl;
		return -70;
	}

	u_long on = 1;

	retval = ioctlsocket(g_ListenSocket, FIONBIO, &on);

	if (retval == SOCKET_ERROR) {
		std::cout << GetLastError() << std::endl;
		return -79;
	}



	std::cout << "Server Start -=-=-=-=-=-=-=-=-==-=-=-=-=-" << std::endl;

	g_GameFrameTick = timeGetTime() - 20;
	g_GameMonitorTick = timeGetTime() - 1000;
	while (!g_bShutdown) {
		++g_LoopCount;

		SocketSet();

		NetWork();

		Update();

		Monitor();

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
			// TODO endcode 있음
			if (user->m_pRecvQ->GetUseSize() < packHeader.bySize + sizeof(st_PACKET_HEADER) + sizeof(dfNETWORK_PACKET_END)) {
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
	BYTE endcode;

	// TODO endcode 만큼 추가
	pPacketUser->m_pRecvQ->DeQueue((char *)&recvHeader, sizeof(recvHeader));
	pPacketUser->m_pRecvQ->DeQueue(pack.GetBufferPtr(), payloadSize);
	pPacketUser->m_pRecvQ->DeQueue((char*)&endcode, sizeof(endcode));
	pack.MoveWritePos(payloadSize);


	//TODO 패킷 메세지 처리부
	switch (packHeader.byType) {
	case dfPACKET_CS_MOVE_START:
		return NetPacket_ReqMoveStart(pPacketUser, &pack);
		break;
		
	case dfPACKET_CS_MOVE_STOP:
		return NetPacket_ReqMoveStop(pPacketUser, &pack);
		break;

	case dfPACKET_CS_ATTACK1:
		return NetPacket_ReqAttack1(pPacketUser, &pack);
		break;

	case dfPACKET_CS_ATTACK2:
		return NetPacket_ReqAttack2(pPacketUser, &pack);
		break;

	case dfPACKET_CS_ATTACK3:
		return NetPacket_ReqAttack3(pPacketUser, &pack);
		break;

	//case dfPACKET_SC_SYNC:
		//return NetPacket_ReqSync(pPacketUser, &pack);
		//break;

	case (BYTE)(252) :
		return NetPacket_ReqEcho(pPacketUser, &pack);
		break;

		
	default:
		return FALSE;
	}
}


void UserDisconnect(User * pClient)
{
	//TODO sector에서 제거 필요
	PacketBuffer pack(15000);

	if (pClient->m_curSector.x != -1 && pClient->m_curSector.y != -1) {

		MakePacket_RemoveCharacter(&pack, pClient);

		Send_Around(pClient, &pack, false);

		SectorRemoveUser(pClient);
	}

	g_UserList.remove(pClient);

	closesocket(pClient->m_socClient);

	delete pClient;
}

void SocketSet()
{
	TIMEVAL tVal;
	int fdSetCount = 0;
	FD_SET readSet;
	FD_SET writeSet;
	int retval;

	tVal.tv_sec = 0;
	tVal.tv_usec = 0;

	FD_ZERO(&readSet);
	FD_ZERO(&writeSet);

	FD_SET(g_ListenSocket, &readSet);

	retval = select(0, &readSet, &writeSet, NULL, &tVal);

	if (retval == SOCKET_ERROR) {
		DWORD errorCode = GetLastError();
		if (errorCode != WSAEWOULDBLOCK) {
			std::cout << errorCode << std::endl;
			g_bShutdown = true;
			return;
		}
	}

	if (FD_ISSET(g_ListenSocket, &readSet)) {
		// connect 시킴

		User *newUser = new User(g_UID++);
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

			NewUserJoin(newUser);
			
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
					g_bShutdown = true;
					return;
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

	iter = g_UserList.begin();
	fdSetCount = 0;
}

void Monitor()
{
	//TODO n초마다 모니터링 할 정보 출력하게 만들기
	DWORD tick = timeGetTime();

	if (tick - g_GameMonitorTick < dfMONITOR_TICK) {
		return;
	}

	g_GameMonitorTick += dfMONITOR_TICK;

	time_t t = time(0);
	tm now;
	localtime_s(&now, &t);

	wprintf(L"[%d-%d-%d %d:%d:%d] Frame %d Loop %d\n", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec, g_FrameCount, g_LoopCount);

	g_FrameCount = 0;
	g_LoopCount = 0;
}

void Update()
{
	//TODO 게임 로직

	//TODO 로직 50프레임 처리
	DWORD tick = timeGetTime();

	if (tick - g_GameFrameTick < 20) {
		return;
	}

	g_FrameCount++;
	g_GameFrameTick += 20;

	User* pUser = nullptr;

	for (auto iter = g_UserList.begin(); iter != g_UserList.end();) {
		pUser = *iter;
		++iter;

/*		if (pUser->m_HP <= 0) {
			//TODO 피없으면 사망처리

			UserDisconnect(pUser);
			continue;
		}

		*/

		switch (pUser->m_action) {
		case dfACTION_MOVE_LL:
			if (CharacterMoveCheck(pUser->m_X - dfSPEED_PLAYER_X, pUser->m_Y)) {
				pUser->m_X -= dfSPEED_PLAYER_X;
			}
			else {
				pUser->m_X = dfRANGE_MOVE_LEFT;
			}
			break;

		case dfACTION_MOVE_LU:
			if (CharacterMoveCheck(pUser->m_X - dfSPEED_PLAYER_X, pUser->m_Y - dfSPEED_PLAYER_Y)) {
				pUser->m_X -= dfSPEED_PLAYER_X;
				pUser->m_Y -= dfSPEED_PLAYER_Y;
			}
			else {
				pUser->m_X = dfRANGE_MOVE_LEFT;
				pUser->m_Y = dfRANGE_MOVE_TOP;
			}
			break;

		case dfACTION_MOVE_UU:
			if (CharacterMoveCheck(pUser->m_X, pUser->m_Y - dfSPEED_PLAYER_Y)) {
				pUser->m_Y -= dfSPEED_PLAYER_Y;
			}
			else {
				pUser->m_Y = dfRANGE_MOVE_TOP;
			}
			break;

		case dfACTION_MOVE_RU:
			if (CharacterMoveCheck(pUser->m_X + dfSPEED_PLAYER_X, pUser->m_Y - dfSPEED_PLAYER_Y)) {
				pUser->m_X += dfSPEED_PLAYER_X;
				pUser->m_Y -= dfSPEED_PLAYER_Y;
			}
			else {
				pUser->m_X = dfRANGE_MOVE_RIGHT;
				pUser->m_Y = dfRANGE_MOVE_TOP;
			}
			break;

		case dfACTION_MOVE_RR:
			if (CharacterMoveCheck(pUser->m_X + dfSPEED_PLAYER_X, pUser->m_Y)) {
				pUser->m_X += dfSPEED_PLAYER_X;
			}
			else {
				pUser->m_X = dfRANGE_MOVE_RIGHT;
			}
			break;

		case dfACTION_MOVE_RD:
			if (CharacterMoveCheck(pUser->m_X + dfSPEED_PLAYER_X, pUser->m_Y + dfSPEED_PLAYER_Y)) {
				pUser->m_X += dfSPEED_PLAYER_X;
				pUser->m_Y += dfSPEED_PLAYER_Y;
			}
			else {
				pUser->m_X = dfRANGE_MOVE_RIGHT;
				pUser->m_Y = dfRANGE_MOVE_BOTTOM;
			}
			break;

		case dfACTION_MOVE_DD:
			if (CharacterMoveCheck(pUser->m_X, pUser->m_Y + dfSPEED_PLAYER_Y)) {
				pUser->m_Y += dfSPEED_PLAYER_Y;
			}
			else {
				pUser->m_Y = dfRANGE_MOVE_BOTTOM;
			}
			break;

		case dfACTION_MOVE_LD:
			if (CharacterMoveCheck(pUser->m_X - dfSPEED_PLAYER_X, pUser->m_Y + dfSPEED_PLAYER_Y)) {
				pUser->m_X -= dfSPEED_PLAYER_X;
				pUser->m_Y += dfSPEED_PLAYER_Y;
			}
			else {
				pUser->m_X = dfRANGE_MOVE_LEFT;
				pUser->m_Y = dfRANGE_MOVE_BOTTOM;
			}
			break;
		}

		
		if (pUser->m_action >= dfACTION_MOVE_LL && pUser->m_action <= dfACTION_MOVE_LD) {
			if (SectorUpdateUser(pUser)) {
				UserSectorUpdatePacket(pUser);
			}
		}
	}
}

BOOL CharacterMoveCheck(int x, int y)
{
	if (x < dfRANGE_MOVE_LEFT)
		return false;
	if (x > dfRANGE_MOVE_RIGHT)
		return false;
	if (y < dfRANGE_MOVE_TOP)
		return false;
	if (y > dfRANGE_MOVE_BOTTOM)
		return false;

	return true;
}
