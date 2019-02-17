#include "stdafx.h"
#include "Network.h"
#include "PlayerObject.h"
#include "StreamQ.h"
#include "PacketDefine.h"
#include "TCPFighter.h"
#include "EffectObject.h"

extern HINSTANCE hInst;
extern HWND g_hWnd;
extern BOOL g_bActiveApp;
extern PlayerObject *g_playerObject;
extern SOCKET g_socket;
extern BOOL g_connect;
extern StreamQ g_recvQ;
extern StreamQ g_sendQ;
extern std::list<PlayerObject *> g_playerList;
extern std::list<EffectObject *> g_effectList;

BOOL PACKET_SC_CREATE_MY_CHARACTER(char * pack)
{
	stPACKET_SC_CREATE_MY_CHARACTER *packet = (stPACKET_SC_CREATE_MY_CHARACTER *)pack;
	
	g_playerObject = new PlayerObject(TRUE);
	if(packet->Direction)
		g_playerObject->SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L03, 5);
	else
		g_playerObject->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R03, 5);
	g_playerObject->SetDirection(packet->Direction);
	g_playerObject->SetCurPosition(packet->X, packet->Y);
	g_playerObject->SetObjectID(packet->ID);
	g_playerObject->SetHP(packet->HP);

	g_playerList.push_back(g_playerObject);

	return TRUE;
}

BOOL PACKET_SC_CREATE_OTHER_CHARACTER(char * pack)
{
	stPACKET_SC_CREATE_OTHER_CHARACTER *packet = (stPACKET_SC_CREATE_OTHER_CHARACTER *)pack;

	PlayerObject *po = new PlayerObject(FALSE);

	if (packet->Direction)
		po->SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L03, 5);
	else
		po->SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R03, 5);
	po->SetDirection(packet->Direction);
	po->SetCurPosition(packet->X, packet->Y);
	po->SetObjectID(packet->ID);
	po->SetHP(packet->HP);
	po->ActionInput(dfACTION_STAND);

	g_playerList.push_back(po);

	return TRUE;
}

BOOL PACKET_SC_DELETE_CHARACTER(char * pack)
{
	stPACKET_SC_DELETE_CHARACTER *packet = (stPACKET_SC_DELETE_CHARACTER *)pack;
	
	for (auto iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		if ((*iter)->GetObjectID() == packet->ID) {
			PlayerObject *po = (*iter);

			g_playerList.erase(iter);

			delete po;

			// 무조건 ID가 고유하다는 가정 하에 더이상 검색 안함
			return TRUE;
		}
	}

	// ID 못찾아서 포문 종료시 리턴 FALSE
	return FALSE;
}

BOOL PACKET_SC_MOVE_START(char * pack)
{
	stPACKET_SC_MOVE_START *packet = (stPACKET_SC_MOVE_START *)pack;

	for (auto iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		if ((*iter)->GetObjectID() == packet->ID) {
			(*iter)->SetCurPosition(packet->X, packet->Y);
			(*iter)->ActionInput(packet->Direction);

			// 무조건 ID가 고유하다는 가정 하에 더이상 검색 안함
			return TRUE;
		}
	}

	return FALSE;
}

BOOL PACKET_SC_MOVE_STOP(char * pack)
{
	stPACKET_SC_MOVE_STOP *packet = (stPACKET_SC_MOVE_STOP *)pack;

	for (auto iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		if ((*iter)->GetObjectID() == packet->ID) {
			(*iter)->ActionInput(dfACTION_STAND);

			// 무조건 ID가 고유하다는 가정 하에 더이상 검색 안함
			return TRUE;
		}
	}

	return FALSE;

}

BOOL PACKET_SC_ATTACK1(char * pack)
{
	stPACKET_SC_ATTACK1 *packet = (stPACKET_SC_ATTACK1 *)pack;

	for (auto iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		if ((*iter)->GetObjectID() == packet->ID) {
			(*iter)->SetDirection(packet->Direction);
			(*iter)->SetCurPosition(packet->X, packet->Y);
			(*iter)->ActionInput(dfACTION_ATTACK1);

			// 무조건 ID가 고유하다는 가정 하에 더이상 검색 안함
			return TRUE;
		}
	}

	return FALSE;
}

BOOL PACKET_SC_ATTACK2(char * pack)
{
	stPACKET_SC_ATTACK2 *packet = (stPACKET_SC_ATTACK2 *)pack;

	for (auto iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		if ((*iter)->GetObjectID() == packet->ID) {
			(*iter)->SetDirection(packet->Direction);
			(*iter)->SetCurPosition(packet->X, packet->Y);
			(*iter)->ActionInput(dfACTION_ATTACK2);

			// 무조건 ID가 고유하다는 가정 하에 더이상 검색 안함
			return TRUE;
		}
	}

	return FALSE;
}

BOOL PACKET_SC_ATTACK3(char * pack)
{
	stPACKET_SC_ATTACK3 *packet = (stPACKET_SC_ATTACK3 *)pack;

	for (auto iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		if ((*iter)->GetObjectID() == packet->ID) {
			(*iter)->SetDirection(packet->Direction);
			(*iter)->SetCurPosition(packet->X, packet->Y);
			(*iter)->ActionInput(dfACTION_ATTACK3);

			// 무조건 ID가 고유하다는 가정 하에 더이상 검색 안함
			return TRUE;
		}
	}

	return FALSE;
}

BOOL PACKET_SC_DAMAGE(char * pack)
{
	stPACKET_SC_DAMAGE *packet = (stPACKET_SC_DAMAGE *)pack;
	
	for (auto iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		if ((*iter)->GetObjectID() == packet->DamageID) {
			(*iter)->SetHP(packet->DamageHP);

			EffectObject *effect = new EffectObject();
			effect->SetCurPosition((*iter)->GetCurX(), (*iter)->GetCurY() - 70);

			g_effectList.push_back(effect);

			if ((*iter) == g_playerObject && packet->DamageHP == 0) {
				MessageBox(g_hWnd, L"패배", L"패배", MB_OK);
				PostQuitMessage(0);
			}

			// 무조건 ID가 고유하다는 가정 하에 더이상 검색 안함
			return TRUE;
		}
	}

	return FALSE;
}

BOOL PACKET_SC_SYNC(char * pack)
{
	stPACKET_SC_SYNC *packet = (stPACKET_SC_SYNC *)pack;

	for (auto iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		if ((*iter)->GetObjectID() == packet->SyncID) {
			(*iter)->SetCurPosition(packet->X, packet->Y);

			return TRUE;
		}
	}

	return FALSE;

}

void PACKET_CS_MOVE_START(st_NETWORK_PACKET_HEADER * header, stPACKET_CS_MOVE_START * packet, BYTE direction, WORD x, WORD y)
{
	header->byCode = dfNETWORK_PACKET_CODE;
	header->bySize = sizeof(stPACKET_CS_MOVE_START);
	header->byType = dfPACKET_CS_MOVE_START;

	packet->Direction = direction;
	packet->X = x;
	packet->Y = y;
}

void PACKET_CS_MOVE_STOP(st_NETWORK_PACKET_HEADER * header, stPACKET_CS_MOVE_STOP * packet, BYTE direction, WORD x, WORD y)
{
	header->byCode = dfNETWORK_PACKET_CODE;
	header->bySize = sizeof(stPACKET_CS_MOVE_STOP);
	header->byType = dfPACKET_CS_MOVE_STOP;

	packet->Direction = direction;
	packet->X = x;
	packet->Y = y;
}

void PACKET_CS_ATTACK1(st_NETWORK_PACKET_HEADER * header, stPACKET_CS_ATTACK1 * packet, BYTE direction, WORD x, WORD y)
{
	header->byCode = dfNETWORK_PACKET_CODE;
	header->bySize = sizeof(stPACKET_CS_ATTACK1);
	header->byType = dfPACKET_CS_ATTACK1;

	packet->Direction = direction;
	packet->X = x;
	packet->Y = y;
}

void PACKET_CS_ATTACK2(st_NETWORK_PACKET_HEADER * header, stPACKET_CS_ATTACK2 * packet, BYTE direction, WORD x, WORD y)
{
	header->byCode = dfNETWORK_PACKET_CODE;
	header->bySize = sizeof(stPACKET_CS_ATTACK2);
	header->byType = dfPACKET_CS_ATTACK2;

	packet->Direction = direction;
	packet->X = x;
	packet->Y = y;
}

void PACKET_CS_ATTACK3(st_NETWORK_PACKET_HEADER * header, stPACKET_CS_ATTACK3 * packet, BYTE direction, WORD x, WORD y)
{
	header->byCode = dfNETWORK_PACKET_CODE;
	header->bySize = sizeof(stPACKET_CS_ATTACK3);
	header->byType = dfPACKET_CS_ATTACK3;

	packet->Direction = direction;
	packet->X = x;
	packet->Y = y;
}

