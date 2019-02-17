#include "stdafx.h"
#include "PlayerObject.h"
#include "TCPFighter.h"
#include "Sprite.h"
#include "EffectObject.h"
#include "TileMap.h"
#include "Network.h"

extern Sprite g_sprite;
extern TileMap g_tileMap;

PlayerObject::PlayerObject(BOOL isPlayer) : m_bPlayerCharacter(isPlayer)
{
	m_eObjectType = ObjectType::Player;
	m_chHP = 0;
	m_dwActionCur = dfACTION_STAND;
	m_dwActionOld = dfACTION_STAND;
	m_iDirCur = 0;
	m_iDirOld = 0;
}


PlayerObject::~PlayerObject()
{
}

void PlayerObject::Action()
{
	NextFrame();
	ActionProc();
}

void PlayerObject::ActionProc()
{
	switch (m_dwActionCur) {
	case dfACTION_ATTACK1:
	case dfACTION_ATTACK2:
	case dfACTION_ATTACK3:
		if (IsEndFrame()) {
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
		}
		break;
	default:
		InputActionProc();
		break;
	}
}

void PlayerObject::Draw(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	int cameraX = m_iCurX - g_tileMap.GetDrawX();
	int cameraY = m_iCurY - g_tileMap.GetDrawY() + 50;

	g_sprite.DrawSprite50(eSHADOW, cameraX, cameraY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	g_sprite.DrawSprite(GetSprite(), cameraX, cameraY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	g_sprite.DrawSprite(eGUAGE_HP, cameraX - 35, cameraY + 9, bypDest, iDestWidth, iDestHeight, iDestPitch, m_chHP);
}

void PlayerObject::SetDirection(int direction)
{
	m_iDirCur = direction;
}

void PlayerObject::SetHP(char hp)
{
	m_chHP = hp;
}

void PlayerObject::ActionInput(DWORD dwAction)
{
	m_dwActionInput = dwAction;
}

void PlayerObject::SetActionAttack1()
{
	if (IsEndFrame()) {
		SetActionStand();
		return;
	}

	if (!m_iDirCur) {
		if (m_iSpriteNow >= ePLAYER_ATTACK1_L01 && m_iSpriteNow <= ePLAYER_ATTACK1_L04)
			return;
		SetSprite(ePLAYER_ATTACK1_L01, ePLAYER_ATTACK1_L04, 3);
	}
	else {
		if (m_iSpriteNow >= ePLAYER_ATTACK1_R01 && m_iSpriteNow <= ePLAYER_ATTACK1_R04)
			return;
		SetSprite(ePLAYER_ATTACK1_R01, ePLAYER_ATTACK1_R04, 3);
	}
}

void PlayerObject::SetActionAttack2()
{
	if (IsEndFrame()) {
		SetActionStand();
		return;
	}

	if (!m_iDirCur) {
		if (m_iSpriteNow >= ePLAYER_ATTACK2_L01 && m_iSpriteNow <= ePLAYER_ATTACK2_L04)
			return;
		SetSprite(ePLAYER_ATTACK2_L01, ePLAYER_ATTACK2_L04, 4);
	}
	else {
		if (m_iSpriteNow >= ePLAYER_ATTACK2_R01 && m_iSpriteNow <= ePLAYER_ATTACK2_R04)
			return;
		SetSprite(ePLAYER_ATTACK2_R01, ePLAYER_ATTACK2_R04, 4);
	}
}

void PlayerObject::SetActionAttack3()
{
	if (IsEndFrame()) {
		SetActionStand();
		return;
	}

	if (!m_iDirCur) {
		if (m_iSpriteNow >= ePLAYER_ATTACK3_L01 && m_iSpriteNow <= ePLAYER_ATTACK3_L06)
			return;
		SetSprite(ePLAYER_ATTACK3_L01, ePLAYER_ATTACK3_L06, 4);
	}
	else {
		if (m_iSpriteNow >= ePLAYER_ATTACK3_R01 && m_iSpriteNow <= ePLAYER_ATTACK3_R06)
			return;
		SetSprite(ePLAYER_ATTACK3_R01, ePLAYER_ATTACK3_R06, 4);
	}
}

void PlayerObject::SetActionMove()
{
	if (!m_iDirCur) {
		if (m_iSpriteNow >= ePLAYER_MOVE_L01 && m_iSpriteNow <= ePLAYER_MOVE_L12)
			return;
		SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L12, 4);
	}
	else {
		if (m_iSpriteNow >= ePLAYER_MOVE_R01 && m_iSpriteNow <= ePLAYER_MOVE_R12)
			return;
		SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R12, 4);
	}
}

void PlayerObject::SetActionStand()
{
	m_dwActionOld = m_dwActionCur;
	m_dwActionCur = dfACTION_STAND;
	m_dwActionInput = dfACTION_STAND;

	if (!m_iDirCur)
	{
		if (m_iSpriteNow >= ePLAYER_STAND_L01 && m_iSpriteNow <= ePLAYER_STAND_L03)
			return;
		SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L03, 5);
	}
	else {
		if (m_iSpriteNow >= ePLAYER_STAND_R01 && m_iSpriteNow <= ePLAYER_STAND_R03)
			return;
		SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R03, 5);
	}
}

void PlayerObject::InputActionProc()
{
	switch(m_dwActionInput) {
	case dfACTION_MOVE_LU: {
		m_iCurX -= dfSPEED_PLAYER_X;
		m_iCurY -= dfSPEED_PLAYER_Y;

		m_iDirCur = FALSE;

		SetActionMove();

		if (m_iCurX <= dfRANGE_MOVE_LEFT)
		{
			SetActionStand();
			m_iCurX = dfRANGE_MOVE_LEFT;
			break;
		}
		if (m_iCurY <= dfRANGE_MOVE_TOP)
		{
			SetActionStand();
			m_iCurY = dfRANGE_MOVE_TOP;
			break;
		}
		if (m_dwActionCur != dfACTION_MOVE_LU) {
			m_dwActionCur = dfACTION_MOVE_LU;
			st_NETWORK_PACKET_HEADER header;
			stPACKET_CS_MOVE_START packet;
			PACKET_CS_MOVE_START(&header, &packet, dfACTION_MOVE_LU, GetCurX(), GetCurY());
			SendPacket(&header, (char *)&packet);
		}
	}
		break;

	case dfACTION_MOVE_LD: {
		m_iCurY += dfSPEED_PLAYER_Y;
		m_iCurX -= dfSPEED_PLAYER_X;

		m_iDirCur = FALSE;
		SetActionMove();

		if (m_iCurX >= dfRANGE_MOVE_RIGHT)
		{
			SetActionStand();
			m_iCurX = dfRANGE_MOVE_RIGHT;
			break;
		}
		if (m_iCurY >= dfRANGE_MOVE_BOTTOM)
		{
			SetActionStand();
			m_iCurY = dfRANGE_MOVE_BOTTOM;
			break;
		}
		if (m_dwActionCur != dfACTION_MOVE_LD) {
			m_dwActionCur = dfACTION_MOVE_LD;
			st_NETWORK_PACKET_HEADER header;
			stPACKET_CS_MOVE_START packet;
			PACKET_CS_MOVE_START(&header, &packet, dfACTION_MOVE_LD, GetCurX(), GetCurY());
			SendPacket(&header, (char *)&packet);
		}
	}
		break;

	case dfACTION_MOVE_LL: {
		m_iCurX -= dfSPEED_PLAYER_X;

		m_iDirCur = FALSE;
		SetActionMove();

		if (m_iCurX <= dfRANGE_MOVE_LEFT)
		{
			SetActionStand();
			m_iCurX = dfRANGE_MOVE_LEFT;
			break;
		}
		if (m_dwActionCur != dfACTION_MOVE_LL) {

			m_dwActionCur = dfACTION_MOVE_LL;
			st_NETWORK_PACKET_HEADER header;
			stPACKET_CS_MOVE_START packet;
			PACKET_CS_MOVE_START(&header, &packet, dfACTION_MOVE_LL, GetCurX(), GetCurY());
			SendPacket(&header, (char *)&packet);
		}
	}
		break;

	case dfACTION_MOVE_RU: {
		m_iCurY -= dfSPEED_PLAYER_Y;
		m_iCurX += dfSPEED_PLAYER_X;

		m_iDirCur = TRUE;
		SetActionMove();

		if (m_iCurX >= dfRANGE_MOVE_RIGHT)
		{
			SetActionStand();
			m_iCurX = dfRANGE_MOVE_RIGHT;
			break;
		}
		if (m_iCurY <= dfRANGE_MOVE_TOP)
		{
			SetActionStand();
			m_iCurY = dfRANGE_MOVE_TOP;
			break;
		}
		if (m_dwActionCur != dfACTION_MOVE_RU) {
			m_dwActionCur = dfACTION_MOVE_RU;
			st_NETWORK_PACKET_HEADER header;
			stPACKET_CS_MOVE_START packet;
			PACKET_CS_MOVE_START(&header, &packet, dfACTION_MOVE_RU, GetCurX(), GetCurY());
			SendPacket(&header, (char *)&packet);
		}
	}
		break;

	case dfACTION_MOVE_RD: {
		m_iCurY += dfSPEED_PLAYER_Y;
		m_iCurX += dfSPEED_PLAYER_X;

		m_iDirCur = TRUE;
		SetActionMove();

		if (m_iCurX <= dfRANGE_MOVE_LEFT)
		{
			SetActionStand();
			m_iCurX = dfRANGE_MOVE_LEFT;
			break;
		}
		if (m_iCurY >= dfRANGE_MOVE_BOTTOM)
		{
			SetActionStand();
			m_iCurY = dfRANGE_MOVE_BOTTOM;
			break;
		}

		if (m_dwActionCur != dfACTION_MOVE_RD) {
			m_dwActionCur = dfACTION_MOVE_RD;
			st_NETWORK_PACKET_HEADER header;
			stPACKET_CS_MOVE_START packet;
			PACKET_CS_MOVE_START(&header, &packet, dfACTION_MOVE_RD, GetCurX(), GetCurY());
			SendPacket(&header, (char *)&packet);
		}

	}
		break;

	case dfACTION_MOVE_RR: {
		m_iCurX += dfSPEED_PLAYER_X;

		m_iDirCur = TRUE;
		SetActionMove();

		if (m_iCurX >= dfRANGE_MOVE_RIGHT)
		{
			SetActionStand();
			m_iCurX = dfRANGE_MOVE_RIGHT;
			break;
		}
		if (m_dwActionCur != dfACTION_MOVE_RR) {
			m_dwActionCur = dfACTION_MOVE_RR;
			st_NETWORK_PACKET_HEADER header;
			stPACKET_CS_MOVE_START packet;
			PACKET_CS_MOVE_START(&header, &packet, dfACTION_MOVE_RR, GetCurX(), GetCurY());
			SendPacket(&header, (char *)&packet);
		}
	}
		break;

	case dfACTION_MOVE_UU: {
		m_iCurY -= dfSPEED_PLAYER_Y;
		SetActionMove();

		if (m_iCurY <= dfRANGE_MOVE_TOP)
		{
			SetActionStand();
			m_iCurY = dfRANGE_MOVE_TOP;
			break;
		}
		if (m_dwActionCur != dfACTION_MOVE_UU) {
			m_dwActionCur = dfACTION_MOVE_UU;
			st_NETWORK_PACKET_HEADER header;
			stPACKET_CS_MOVE_START packet;
			PACKET_CS_MOVE_START(&header, &packet, dfACTION_MOVE_UU, GetCurX(), GetCurY());
			SendPacket(&header, (char *)&packet);
		}
	}
		break;

	case dfACTION_MOVE_DD: {
		m_iCurY += dfSPEED_PLAYER_Y;
		SetActionMove();

		if (m_iCurY >= dfRANGE_MOVE_BOTTOM)
		{
			SetActionStand();
			m_iCurY = dfRANGE_MOVE_BOTTOM;
			break;
		}
		if (m_dwActionCur != dfACTION_MOVE_DD) {
			m_dwActionCur = dfACTION_MOVE_DD;
			st_NETWORK_PACKET_HEADER header;
			stPACKET_CS_MOVE_START packet;
			PACKET_CS_MOVE_START(&header, &packet, dfACTION_MOVE_DD, GetCurX(), GetCurY());
			SendPacket(&header, (char *)&packet);
		}
	}
		break;

	case dfACTION_ATTACK1: {
		SetActionAttack1();
		if (m_dwActionCur != dfACTION_ATTACK1) {
			m_dwActionCur = dfACTION_ATTACK1;
			st_NETWORK_PACKET_HEADER header;
			stPACKET_CS_MOVE_STOP packet;
			stPACKET_CS_ATTACK1 attackPacket;
			if (GetDirection())
				PACKET_CS_MOVE_STOP(&header, &packet, dfACTION_MOVE_RR, GetCurX(), GetCurY());
			else
				PACKET_CS_MOVE_STOP(&header, &packet, dfACTION_MOVE_LL, GetCurX(), GetCurY());
			SendPacket(&header, (char *)&packet);

			if (GetDirection())
				PACKET_CS_ATTACK1(&header, &attackPacket, dfACTION_MOVE_RR, GetCurX(), GetCurY());
			else
				PACKET_CS_ATTACK1(&header, &attackPacket, dfACTION_MOVE_LL, GetCurX(), GetCurY());
			SendPacket(&header, (char *)&attackPacket);
		}
	}
		break;
	case dfACTION_ATTACK2: {
		SetActionAttack2();
		if (m_dwActionCur != dfACTION_ATTACK2) {
			m_dwActionCur = dfACTION_ATTACK2;
			st_NETWORK_PACKET_HEADER header;
			stPACKET_CS_MOVE_STOP packet;
			stPACKET_CS_ATTACK2 attackPacket;
			if (GetDirection())
				PACKET_CS_MOVE_STOP(&header, &packet, dfACTION_MOVE_RR, GetCurX(), GetCurY());
			else
				PACKET_CS_MOVE_STOP(&header, &packet, dfACTION_MOVE_LL, GetCurX(), GetCurY());
			SendPacket(&header, (char *)&packet);

			if (GetDirection())
				PACKET_CS_ATTACK2(&header, &attackPacket, dfACTION_MOVE_RR, GetCurX(), GetCurY());
			else
				PACKET_CS_ATTACK2(&header, &attackPacket, dfACTION_MOVE_LL, GetCurX(), GetCurY());
			SendPacket(&header, (char *)&attackPacket);
		}
	}
		break;
	case dfACTION_ATTACK3: {
		SetActionAttack3();

		if (m_dwActionCur != dfACTION_ATTACK3) {
			m_dwActionCur = dfACTION_ATTACK3;
			st_NETWORK_PACKET_HEADER header;
			stPACKET_CS_MOVE_STOP packet;
			stPACKET_CS_ATTACK3 attackPacket;
			if (GetDirection())
				PACKET_CS_MOVE_STOP(&header, &packet, dfACTION_MOVE_RR, GetCurX(), GetCurY());
			else
				PACKET_CS_MOVE_STOP(&header, &packet, dfACTION_MOVE_LL, GetCurX(), GetCurY());
			SendPacket(&header, (char *)&packet);

			if (GetDirection())
				PACKET_CS_ATTACK3(&header, &attackPacket, dfACTION_MOVE_RR, GetCurX(), GetCurY());
			else
				PACKET_CS_ATTACK3(&header, &attackPacket, dfACTION_MOVE_LL, GetCurX(), GetCurY());
			SendPacket(&header, (char *)&attackPacket);
		}
	}
		break;
	case dfACTION_STAND: {
		if (m_dwActionCur != dfACTION_STAND) {
			SetActionStand();
			st_NETWORK_PACKET_HEADER header;
			stPACKET_CS_MOVE_STOP packet;
			if (GetDirection())
				PACKET_CS_MOVE_STOP(&header, &packet, dfACTION_MOVE_RR, GetCurX(), GetCurY());
			else
				PACKET_CS_MOVE_STOP(&header, &packet, dfACTION_MOVE_LL, GetCurX(), GetCurY());
			SendPacket(&header, (char *)&packet);
		}
	}
		break;

	default:
		break;
	}
}
