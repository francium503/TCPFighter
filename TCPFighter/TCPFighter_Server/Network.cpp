#include "pch.h"
#include "User.h"
#include "PacketBuffer.h"
#include "Protocol.h"
#include "Network.h"
#include "StreamQ.h"
#include "Sector.h"


extern std::list<User *> g_Sector[dfSECTOR_MAX_Y][dfSECTOR_MAX_X];



BOOL NetPacket_ReqMoveStart(User * packUser, PacketBuffer * pPackBuffer)
{
	BYTE direction;
	WORD x;
	WORD y;
	PacketBuffer sendPack(15000);

	*pPackBuffer >> direction >> x >> y;

	//TODO 데드레커닝 처리
	if ( abs(packUser->m_X - x) > dfERROR_RANGE || abs(packUser->m_Y - y) > dfERROR_RANGE) {
		int drX = packUser->m_X;
		int drY = packUser->m_Y;

		int frame = DeadReckoningPos(packUser->m_action, packUser->m_lastActionTime, packUser->m_lastActionX, packUser->m_lastActionY, &drX, &drY);

		if (abs(drX - x) > dfERROR_RANGE || abs(drY - y) > dfERROR_RANGE) {
			packUser->m_X = drX;
			packUser->m_Y = drY;
			MakePacket_Sync(&sendPack, packUser);
			Send_Around(packUser, &sendPack, true);

		}
		x = drX;
		y = drY;
	}


	packUser->m_action = direction;

	switch (direction) {
	case dfPACKET_MOVE_DIR_RR:
	case dfPACKET_MOVE_DIR_RD:
	case dfPACKET_MOVE_DIR_RU:
		packUser->m_direction = dfPACKET_MOVE_DIR_RR;
		break;

	case dfPACKET_MOVE_DIR_LL:
	case dfPACKET_MOVE_DIR_LD:
	case dfPACKET_MOVE_DIR_LU:
		packUser->m_direction = dfPACKET_MOVE_DIR_LL;
		break;

	default:
		break;
	}

	packUser->m_X = x;
	packUser->m_Y = y;

	//TODO 섹터 이동 처리
	if (SectorUpdateUser(packUser)) {
		UserSectorUpdatePacket(packUser);
	}

	packUser->m_lastActionTime = timeGetTime();
	packUser->m_lastActionX = x;
	packUser->m_lastActionY = y;

	MakePacket_MoveStart(&sendPack, packUser);

	Send_Around(packUser, &sendPack, true);
	
	return TRUE;
}

BOOL NetPacket_ReqMoveStop(User * packUser, PacketBuffer * pPackBuffer)
{
	BYTE direction;
	WORD x;
	WORD y;
	PacketBuffer sendPack(15000);

	*pPackBuffer >> direction >> x >> y;

	if (abs(packUser->m_X - x) > dfERROR_RANGE || abs(packUser->m_Y - y) > dfERROR_RANGE) {
		int drX = packUser->m_X;
		int drY = packUser->m_Y;

		int frame = DeadReckoningPos(packUser->m_action, packUser->m_lastActionTime, packUser->m_lastActionX, packUser->m_lastActionY, &drX, &drY);

		if (abs(drX - x) > dfERROR_RANGE || abs(drY - y) > dfERROR_RANGE) {
			packUser->m_X = drX;
			packUser->m_Y = drY;
			MakePacket_Sync(&sendPack, packUser);
			Send_Around(packUser, &sendPack, true);
		}
		x = drX;
		y = drY;
	}

	switch (packUser->m_action) {
	case dfPACKET_MOVE_DIR_RR:
	case dfPACKET_MOVE_DIR_RD:
	case dfPACKET_MOVE_DIR_RU:
		packUser->m_direction = dfPACKET_MOVE_DIR_RR;
		break;

	case dfPACKET_MOVE_DIR_LL:
	case dfPACKET_MOVE_DIR_LD:
	case dfPACKET_MOVE_DIR_LU:
		packUser->m_direction = dfPACKET_MOVE_DIR_LL;
		break;

	default:
		break;

	}
	packUser->m_action = dfACTION_STAND;
	packUser->m_X = x;
	packUser->m_Y = y;

	if (SectorUpdateUser(packUser)) {
		UserSectorUpdatePacket(packUser);
	}

	packUser->m_lastActionTime = timeGetTime();
	packUser->m_lastActionX = x;
	packUser->m_lastActionY = y;

	MakePacket_MoveStop(&sendPack, packUser);

	Send_Around(packUser, &sendPack, true);

	return TRUE;
}

BOOL NetPacket_ReqAttack1(User * packUser, PacketBuffer * pPackBuffer)
{
	BYTE direction;
	WORD x;
	WORD y;

	*pPackBuffer >> direction >> x >> y;

	std::list<User*> *sector = &g_Sector[packUser->m_curSector.y][packUser->m_curSector.x];

	PacketBuffer pack(15000);
	
	MakePacket_Attack1(&pack, packUser);

	Send_Around(packUser, &pack, true);

	auto iter = sector->begin();

	while (iter != sector->end()) {
		if (isHit(packUser, (*iter))) {
			if (packUser == (*iter)) {
				++iter;
				continue;
			}

			if ((*iter)->m_HP >= dfATTACK1_DAMAGE)
				(*iter)->m_HP -= dfATTACK1_DAMAGE;
			else
				(*iter)->m_HP = 0;

			MakePacket_Damage(&pack, packUser, *iter);

			Send_Around(*iter, &pack, true);

		}
		++iter;
	}

	return TRUE;
}

BOOL NetPacket_ReqAttack2(User * packUser, PacketBuffer * pPackBuffer)
{
	BYTE direction;
	WORD x;
	WORD y;

	*pPackBuffer >> direction >> x >> y;

	std::list<User*> *sector = &g_Sector[packUser->m_curSector.y][packUser->m_curSector.x];

	PacketBuffer pack(15000);

	MakePacket_Attack2(&pack, packUser);

	Send_Around(packUser, &pack, true);

	auto iter = sector->begin();

	while (iter != sector->end()) {
		if (isHit(packUser, (*iter))) {
			if (packUser == (*iter)) {
				++iter;
				continue;
			}

			if ((*iter)->m_HP >= dfATTACK2_DAMAGE)
				(*iter)->m_HP -= dfATTACK2_DAMAGE;
			else
				(*iter)->m_HP = 0;

			MakePacket_Damage(&pack, packUser, *iter);

			Send_Around(*iter, &pack, true);

		}
		++iter;
	}

	return TRUE;
}

BOOL NetPacket_ReqAttack3(User * packUser, PacketBuffer * pPackBuffer)
{
	BYTE direction;
	WORD x;
	WORD y;

	*pPackBuffer >> direction >> x >> y;

	std::list<User*> *sector = &g_Sector[packUser->m_curSector.y][packUser->m_curSector.x];

	PacketBuffer pack(15000);

	MakePacket_Attack3(&pack, packUser);

	Send_Around(packUser, &pack, true);

	auto iter = sector->begin();

	while (iter != sector->end()) {
		if (isHit(packUser, (*iter))) {
			if (packUser == (*iter)) {
				++iter;
				continue;
			}

			if ((*iter)->m_HP >= dfATTACK3_DAMAGE)
				(*iter)->m_HP -= dfATTACK3_DAMAGE;
			else
				(*iter)->m_HP = 0;

			MakePacket_Damage(&pack, packUser, *iter);

			Send_Around(*iter, &pack, true);
		}
		++iter;
	}

	return TRUE;
}

BOOL NetPacket_ReqSync(User * packUser, PacketBuffer * pPackBuffer)
{

	PacketBuffer pack(15000);

	MakePacket_Sync(&pack, packUser);

	Send_Unicast(packUser, &pack);

	return TRUE;
}

BOOL NetPacket_ReqEcho(User * packUser, PacketBuffer * pPackBuffer)
{
	PacketBuffer pack(15000);

	st_PACKET_HEADER packHeader;

	packHeader.byCode = dfNETWORK_PACKET_CODE;
	packHeader.bySize = 4;
	packHeader.byType = (BYTE)(253);

	pack.Clear();
	pack.PutData((char *)&packHeader, sizeof(st_PACKET_HEADER));
	pack.PutData(pPackBuffer->GetBufferPtr(), pPackBuffer->GetDataSize());

	pack << dfNETWORK_PACKET_END;

	Send_Unicast(packUser, &pack);

	return TRUE;
}




void MakePacket_MoveStart(PacketBuffer * pack, User * moveUser)
{
	st_PACKET_HEADER packHeader;

	packHeader.byCode = dfNETWORK_PACKET_CODE;
	packHeader.bySize = 9;
	packHeader.byType = dfPACKET_SC_MOVE_START;

	pack->Clear();
	pack->PutData((char *)&packHeader, sizeof(st_PACKET_HEADER));
	*pack << moveUser->m_uID << moveUser->m_action << moveUser->m_X << moveUser->m_Y << dfNETWORK_PACKET_END;
}

void MakePacket_MoveStop(PacketBuffer * pack, User * stopUser)
{
	st_PACKET_HEADER packHeader;

	packHeader.byCode = dfNETWORK_PACKET_CODE;
	packHeader.bySize = 9;
	packHeader.byType = dfPACKET_SC_MOVE_STOP;

	pack->Clear();
	pack->PutData((char *)&packHeader, sizeof(st_PACKET_HEADER));
	*pack << stopUser->m_uID << stopUser->m_direction << stopUser->m_X << stopUser->m_Y << dfNETWORK_PACKET_END;
}

void MakePacket_Sync(PacketBuffer * pack, User * syncUser)
{
	st_PACKET_HEADER packHeader;

	packHeader.byCode = dfNETWORK_PACKET_CODE;
	packHeader.bySize = 8;
	packHeader.byType = dfPACKET_SC_SYNC;

	pack->Clear();
	pack->PutData((char *)&packHeader, sizeof(st_PACKET_HEADER));
	*pack << syncUser->m_uID << syncUser->m_X << syncUser->m_Y << dfNETWORK_PACKET_END;
}

void MakePacket_CreateMyCharacter(PacketBuffer * pack, User * createUser)
{
	st_PACKET_HEADER packHeader;

	packHeader.byCode = dfNETWORK_PACKET_CODE;
	packHeader.bySize = 10;
	packHeader.byType = dfPACKET_SC_CREATE_MY_CHARACTER;

	pack->Clear();

	pack->PutData((char *)&packHeader, sizeof(st_PACKET_HEADER));
	*pack << createUser->m_uID << createUser->m_direction << createUser->m_X << createUser->m_Y << createUser->m_HP << dfNETWORK_PACKET_END;

}

void MakePacket_CreateOtherCharacter(PacketBuffer * pack, User * createUser)
{
	st_PACKET_HEADER packHeader;

	packHeader.byCode = dfNETWORK_PACKET_CODE;
	packHeader.bySize = 10;
	packHeader.byType = dfPACKET_SC_CREATE_OTHER_CHARACTER;

	pack->Clear();

	pack->PutData((char *)&packHeader, sizeof(st_PACKET_HEADER));
	*pack << createUser->m_uID<< createUser->m_direction << createUser->m_X << createUser->m_Y << createUser->m_HP << dfNETWORK_PACKET_END;
}

void MakePacket_RemoveCharacter(PacketBuffer * pack, User * deleteUser)
{
	st_PACKET_HEADER packHeader;

	packHeader.byCode = dfNETWORK_PACKET_CODE;
	packHeader.bySize = 4;
	packHeader.byType = dfPACKET_SC_DELETE_CHARACTER;

	pack->Clear();
	
	pack->PutData((char *)&packHeader, sizeof(st_PACKET_HEADER));
	*pack << deleteUser->m_uID << dfNETWORK_PACKET_END;
}

void MakePacket_Attack1(PacketBuffer * pack, User * attackUser)
{
	st_PACKET_HEADER packHeader;

	packHeader.byCode = dfNETWORK_PACKET_CODE;
	packHeader.bySize = 9;
	packHeader.byType = dfPACKET_SC_ATTACK1;

	pack->Clear();

	pack->PutData((char *)&packHeader, sizeof(st_PACKET_HEADER));
	*pack << attackUser->m_uID << attackUser->m_direction << attackUser->m_X << attackUser->m_Y << dfNETWORK_PACKET_END;
}

void MakePacket_Attack2(PacketBuffer * pack, User * attackUser)
{
	st_PACKET_HEADER packHeader;

	packHeader.byCode = dfNETWORK_PACKET_CODE;
	packHeader.bySize = 9;
	packHeader.byType = dfPACKET_SC_ATTACK2;

	pack->Clear();

	pack->PutData((char *)&packHeader, sizeof(st_PACKET_HEADER));
	*pack << attackUser->m_uID << attackUser->m_direction << attackUser->m_X << attackUser->m_Y << dfNETWORK_PACKET_END;
}

void MakePacket_Attack3(PacketBuffer * pack, User * attackUser)
{
	st_PACKET_HEADER packHeader;

	packHeader.byCode = dfNETWORK_PACKET_CODE;
	packHeader.bySize = 9;
	packHeader.byType = dfPACKET_SC_ATTACK3;

	pack->Clear();

	pack->PutData((char *)&packHeader, sizeof(st_PACKET_HEADER));
	*pack << attackUser->m_uID << attackUser->m_direction << attackUser->m_X << attackUser->m_Y << dfNETWORK_PACKET_END;
}

void MakePacket_Damage(PacketBuffer * pack, User * attackUser, User * damagedUser)
{
	st_PACKET_HEADER packHeader;

	packHeader.byCode = dfNETWORK_PACKET_CODE;
	packHeader.bySize = 9;
	packHeader.byType = dfPACKET_SC_DAMAGE;

	pack->Clear();

	pack->PutData((char *)&packHeader, sizeof(st_PACKET_HEADER));
	*pack << attackUser->m_uID << damagedUser->m_uID << damagedUser->m_HP << dfNETWORK_PACKET_END;
}




void NewUserJoin(User * pUser)
{
	PacketBuffer pack(15000);

	pUser->m_X = (rand() % (dfRANGE_MOVE_RIGHT - 1)) + 1;
	pUser->m_Y = (rand() % (dfRANGE_MOVE_BOTTOM - 1)) + 1;
	pUser->m_action = dfACTION_STAND;
	pUser->m_direction = dfACTION_MOVE_LL;
	pUser->m_lastActionTime = timeGetTime();
	pUser->m_lastActionX = pUser->m_X;
	pUser->m_lastActionY = pUser->m_Y;

	SectorAddUser(pUser);

	MakePacket_CreateMyCharacter(&pack, pUser);

	Send_Unicast(pUser, &pack);
	MakePacket_CreateOtherCharacter(&pack, pUser);

	Send_Around(pUser, &pack, false);

	st_SECTOR_AROUND around;

	GetSectorAround(pUser->m_curSector.x, pUser->m_curSector.y, &around);

	for (int cnt = 0; cnt < around.Count; ++cnt) {
		auto sector = g_Sector[around.Around[cnt].y][around.Around[cnt].x];
		for (auto iter = sector.begin(); iter != sector.end(); ++iter) {
			if ((*iter) == pUser)
				continue;

			MakePacket_CreateOtherCharacter(&pack, *iter);

			Send_Unicast(pUser, &pack);

			switch ((*iter)->m_action) {

			case dfACTION_MOVE_DD:
			case dfACTION_MOVE_LD:
			case dfACTION_MOVE_RD:
			case dfACTION_MOVE_RR:
			case dfACTION_MOVE_LL:
			case dfACTION_MOVE_RU:
			case dfACTION_MOVE_LU:
			case dfACTION_MOVE_UU:
				MakePacket_MoveStart(&pack, (*iter));

				Send_Unicast(pUser, &pack);
				break;
			}
		}
	}
}

void UserSectorUpdatePacket(User * pUser)
{
	st_SECTOR_AROUND removeSec, addSec;
	User* pExistUser;

	std::list<User*>* sectorList;
	std::list<User*>::iterator iter;


	PacketBuffer pack(15000);
	int cnt;

	GetUpdateSectorAround(pUser, &removeSec, &addSec);

	MakePacket_RemoveCharacter(&pack, pUser);

	for (cnt = 0; cnt < removeSec.Count; ++cnt) {
		Send_Sector(&pack, removeSec.Around->x, removeSec.Around->y);
	}

	for (cnt = 0; cnt < removeSec.Count; ++cnt) {
		sectorList = &g_Sector[removeSec.Around[cnt].y][removeSec.Around[cnt].x];

		for (iter = sectorList->begin(); iter != sectorList->end(); ++iter) {
			MakePacket_RemoveCharacter(&pack, (*iter));

			Send_Unicast(pUser, &pack);
		}
	}

	MakePacket_CreateOtherCharacter(&pack, pUser);

	for (cnt = 0; cnt < addSec.Count; ++cnt) {
		Send_Sector(&pack, addSec.Around[cnt].x, addSec.Around[cnt].y);
	}

	if (pUser->m_action == dfACTION_STAND)
		MakePacket_MoveStop(&pack, pUser);
	else {
		MakePacket_MoveStart(&pack, pUser);
	}
	
	for (cnt = 0; cnt < addSec.Count; ++cnt) {
		Send_Sector(&pack, addSec.Around[cnt].x, addSec.Around[cnt].y);
	}

	for (cnt = 0; cnt < addSec.Count; ++cnt) {
		sectorList = &g_Sector[addSec.Around[cnt].y][addSec.Around[cnt].x];

		for (iter = sectorList->begin(); iter != sectorList->end(); ++iter) {
			pExistUser = *iter;

			if (pExistUser != pUser) {
				MakePacket_CreateOtherCharacter(&pack, pExistUser);

				Send_Unicast(pUser, &pack);

				switch (pExistUser->m_action) {
				case dfACTION_MOVE_DD:
				case dfACTION_MOVE_LD:
				case dfACTION_MOVE_RD:
				case dfACTION_MOVE_RR:
				case dfACTION_MOVE_LL:
				case dfACTION_MOVE_RU:
				case dfACTION_MOVE_LU:
				case dfACTION_MOVE_UU:
					MakePacket_MoveStart(&pack, pExistUser);

					Send_Unicast(pUser, &pack);
					break;

				case dfACTION_STAND:
					MakePacket_MoveStop(&pack, pExistUser);

					Send_Unicast(pUser, &pack);
					break;

				case dfACTION_ATTACK1:
				case dfACTION_ATTACK2:
				case dfACTION_ATTACK3:
					break;
				}
			}
		}
	}
}

int DeadReckoningPos(DWORD dwAction, DWORD dwActionTick, int iOldPosX, int iOldPosY, int * pPosX, int * pPosY)
{
	DWORD intervalTick = timeGetTime() - dwActionTick;
	int actionFrame = intervalTick / 20;
	int removeFrame = 0;
	int value;
	int rPosX = iOldPosX;
	int rPosY = iOldPosY;

	int dx = actionFrame * dfSPEED_PLAYER_X;
	int dy = actionFrame * dfSPEED_PLAYER_Y;

	switch (dwAction) {
	case dfACTION_MOVE_LL:
		rPosX = iOldPosX - dx;
		rPosY = iOldPosY;
		break;

	case dfACTION_MOVE_LU:
		rPosX = iOldPosX - dx;
		rPosY = iOldPosY - dy;
		break;

	case dfACTION_MOVE_UU:
		rPosX = iOldPosX;
		rPosY = iOldPosY - dy;
		break;

	case dfACTION_MOVE_RU:
		rPosX = iOldPosX + dx;
		rPosY = iOldPosY - dy;
		break;

	case dfACTION_MOVE_RR:
		rPosX = iOldPosX + dx;
		rPosY = iOldPosY;
		break;

	case dfACTION_MOVE_RD:
		rPosX = iOldPosX + dx;
		rPosY = iOldPosY + dy;
		break;

	case dfACTION_MOVE_DD:
		rPosX = iOldPosX;
		rPosY = iOldPosY + dy;
		break;

	case dfACTION_MOVE_LD:
		rPosX = iOldPosX - dx;
		rPosY = iOldPosY + dy;
		break;
	}


	if (rPosX <= dfRANGE_MOVE_LEFT) {
		value = abs(dfRANGE_MOVE_LEFT - abs(rPosX)) / dfSPEED_PLAYER_X;
		removeFrame = max(value, removeFrame);
	}
	if (rPosX >= dfRANGE_MOVE_RIGHT) {
		value = abs(dfRANGE_MOVE_RIGHT - abs(rPosX)) / dfSPEED_PLAYER_X;
		removeFrame = max(value, removeFrame);
	}
	if (rPosY <= dfRANGE_MOVE_TOP) {
		value = abs(dfRANGE_MOVE_TOP - abs(rPosY)) / dfSPEED_PLAYER_Y;
		removeFrame = max(value, removeFrame);
	}
	if (rPosY >= dfRANGE_MOVE_BOTTOM) {
		value = abs(dfRANGE_MOVE_BOTTOM - abs(rPosY)) / dfSPEED_PLAYER_Y;
		removeFrame = max(value, removeFrame);
	}


	if (removeFrame > 0) {
		actionFrame -= removeFrame;

		dx = actionFrame * dfSPEED_PLAYER_X;
		dy = actionFrame * dfSPEED_PLAYER_Y;

		switch (dwAction) {
		case dfACTION_MOVE_LL:
			rPosX = iOldPosX - dx;
			rPosY = iOldPosY;
			break;

		case dfACTION_MOVE_LU:
			rPosX = iOldPosX - dx;
			rPosY = iOldPosY - dy;
			break;

		case dfACTION_MOVE_UU:
			rPosX = iOldPosX;
			rPosY = iOldPosY - dy;
			break;

		case dfACTION_MOVE_RU:
			rPosX = iOldPosX + dx;
			rPosY = iOldPosY - dy;
			break;

		case dfACTION_MOVE_RR:
			rPosX = iOldPosX + dx;
			rPosY = iOldPosY;
			break;

		case dfACTION_MOVE_RD:
			rPosX = iOldPosX + dx;
			rPosY = iOldPosY + dy;
			break;

		case dfACTION_MOVE_DD:
			rPosX = iOldPosX;
			rPosY = iOldPosY + dy;
			break;

		case dfACTION_MOVE_LD:
			rPosX = iOldPosX - dx;
			rPosY = iOldPosY + dy;
			break;
		}
	}

	rPosX = min(rPosX, dfRANGE_MOVE_RIGHT);
	rPosX = max(rPosX, dfRANGE_MOVE_LEFT);
	rPosY = min(rPosY, dfRANGE_MOVE_BOTTOM);
	rPosY = max(rPosY, dfRANGE_MOVE_TOP);

	*pPosX = rPosX;
	*pPosY = rPosY;

	return actionFrame;
}

BOOL isHit(User * attacker, User * defender)
{
	if (attacker->m_Y + 5 >= defender->m_Y && attacker->m_Y - 5 <= defender->m_Y) {
		if (attacker->m_direction == dfACTION_MOVE_LL) {
			if (attacker->m_X - dfATTACK_RANGE <= defender->m_X && attacker->m_X >= defender->m_X)
				return TRUE;
		}
		else if(attacker->m_direction == dfACTION_MOVE_RR){
			if (attacker->m_X + dfATTACK_RANGE >= defender->m_X && attacker->m_X <= defender->m_X)
				return TRUE;
		}
	}

	return FALSE;
}

void Send_Unicast(User * pClient, PacketBuffer * resPack)
{
	if (pClient == nullptr)
		return;

	pClient->m_pSendQ->Enqueue(resPack->GetBufferPtr(), resPack->GetDataSize());
}

void Send_Sector(PacketBuffer * resPack, int sectorX, int sectorY)
{
	std::list<User*> *sec = &g_Sector[sectorY][sectorX];

	for (auto iter = sec->begin(); iter != sec->end(); ++iter) {
		Send_Unicast(*iter, resPack);
	}
}


void Send_Around(User * pClient, PacketBuffer * resPack, bool includeClient)
{
	int secx = pClient->m_curSector.x;
	int secy = pClient->m_curSector.y;
	
	st_SECTOR_AROUND around;
	std::list<User*>* sectorList;

	GetSectorAround(secx, secy, &around);

	for (int i = 0; i < around.Count; ++i) {
		sectorList = &g_Sector[around.Around[i].y][around.Around[i].x];

		for (auto iter = sectorList->begin(); iter != sectorList->end(); ++iter) {
			if (pClient == *iter && includeClient == false)
				continue;

			Send_Unicast(*iter, resPack);
		}
	}
}
