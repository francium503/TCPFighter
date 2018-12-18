#include "stdafx.h"
#include "Network.h"
#include "PlayerObject.h"
#include "StreamQ.h"
#include "PacketDefine.h"
#include "LinkedList.h"
#include "TCPFighter.h"

extern HINSTANCE hInst;
extern HWND g_hWnd;
extern BOOL g_bActiveApp;
extern PlayerObject *g_playerObject;
extern SOCKET g_socket;
extern BOOL g_connect;
extern StreamQ g_recvQ;
extern StreamQ g_sendQ;
extern List<BaseObject *> g_playerList;

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

	g_playerList.push_back((BaseObject *)po);

	return TRUE;
}

BOOL PACKET_SC_DELETE_CHARACTER(char * pack)
{
	stPACKET_SC_DELETE_CHARACTER *packet = (stPACKET_SC_DELETE_CHARACTER *)pack;

	if (packet->ID == g_playerObject->GetObjectID()) {
		delete[] g_playerObject;
		return TRUE;
	}

	for (List<BaseObject *>::iterator iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		if ((*iter)->GetObjectID() == packet->ID) {
			PlayerObject *po = (PlayerObject *)(*iter);

			g_playerList.erase(iter);

			// List���� erase �Ҷ� ���� ���� �������� ���� delete �� ���� List ���� �ʿ�
			delete po;

			// ������ ID�� �����ϴٴ� ���� �Ͽ� ���̻� �˻� ����
			return TRUE;
		}
	}

	// ID ��ã�Ƽ� ���� ����� ���� FALSE
	return FALSE;
}

BOOL PACKET_SC_MOVE_START(char * pack)
{
	stPACKET_SC_MOVE_START *packet = (stPACKET_SC_MOVE_START *)pack;

	for (List<BaseObject *>::iterator iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		if ((*iter)->GetObjectID() == packet->ID) {
			PlayerObject *po = (PlayerObject *)(*iter);

			po->SetCurPosition(packet->X, packet->Y);
			po->ActionInput(packet->Direction);

			// ������ ID�� �����ϴٴ� ���� �Ͽ� ���̻� �˻� ����
			return TRUE;
		}
	}

	return FALSE;
}

BOOL PACKET_SC_MOVE_STOP(char * pack)
{
	stPACKET_SC_MOVE_STOP *packet = (stPACKET_SC_MOVE_STOP *)pack;

	for (List<BaseObject *>::iterator iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		if ((*iter)->GetObjectID() == packet->ID) {
			PlayerObject *po = (PlayerObject *)(*iter);

			po->ActionInput(dfACTION_STAND);

			// ������ ID�� �����ϴٴ� ���� �Ͽ� ���̻� �˻� ����
			return TRUE;
		}
	}

	return FALSE;

}

BOOL PACKET_SC_ATTACK1(char * pack)
{
	stPACKET_SC_ATTACK1 *packet = (stPACKET_SC_ATTACK1 *)pack;

	for (List<BaseObject *>::iterator iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		if ((*iter)->GetObjectID() == packet->ID) {
			PlayerObject *po = (PlayerObject *)(*iter);
			
			po->SetDirection(packet->Direction);
			po->SetCurPosition(packet->X, packet->Y);
			po->ActionInput(dfACTION_ATTACK1);

			// ������ ID�� �����ϴٴ� ���� �Ͽ� ���̻� �˻� ����
			return TRUE;
		}
	}

	return FALSE;
}

BOOL PACKET_SC_ATTACK2(char * pack)
{
	stPACKET_SC_ATTACK2 *packet = (stPACKET_SC_ATTACK2 *)pack;

	for (List<BaseObject *>::iterator iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		if ((*iter)->GetObjectID() == packet->ID) {
			PlayerObject *po = (PlayerObject *)(*iter);

			po->SetDirection(packet->Direction);
			po->SetCurPosition(packet->X, packet->Y);
			po->ActionInput(dfACTION_ATTACK2);

			// ������ ID�� �����ϴٴ� ���� �Ͽ� ���̻� �˻� ����
			return TRUE;
		}
	}

	return FALSE;
}

BOOL PACKET_SC_ATTACK3(char * pack)
{
	stPACKET_SC_ATTACK3 *packet = (stPACKET_SC_ATTACK3 *)pack;

	for (List<BaseObject *>::iterator iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		if ((*iter)->GetObjectID() == packet->ID) {
			PlayerObject *po = (PlayerObject *)(*iter);

			po->SetDirection(packet->Direction);
			po->SetCurPosition(packet->X, packet->Y);
			po->ActionInput(dfACTION_ATTACK3);

			// ������ ID�� �����ϴٴ� ���� �Ͽ� ���̻� �˻� ����
			return TRUE;
		}
	}

	return FALSE;
}

BOOL PACKET_SC_DAMAGE(char * pack)
{
	stPACKET_SC_DAMAGE *packet = (stPACKET_SC_DAMAGE *)pack;

	if (packet->DamageID == g_playerObject->GetObjectID()) {
		g_playerObject->SetHP(packet->DamageHP);

		return TRUE;
	}

	for (List<BaseObject *>::iterator iter = g_playerList.begin(); iter != g_playerList.end(); ++iter) {
		if ((*iter)->GetObjectID() == packet->DamageID) {
			PlayerObject *po = (PlayerObject *)(*iter);

			po->SetHP(packet->DamageHP);

			// ������ ID�� �����ϴٴ� ���� �Ͽ� ���̻� �˻� ����
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

