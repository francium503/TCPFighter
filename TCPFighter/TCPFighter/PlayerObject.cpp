#include "stdafx.h"
#include "PlayerObject.h"
#include "TCPFighter.h"
#include "Sprite.h"
#include "LinkedList.h"
#include "EffectObject.h"

extern Sprite g_sprite;

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
	switch(m_dwActionCur)
	{
	case dfACTION_MOVE_LL:
		m_iCurX -= dfSPEED_PLAYER_X;

		m_iDirCur = FALSE;
		SetActionMove();

		if (m_iCurX <= dfRANGE_MOVE_LEFT)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurX = dfRANGE_MOVE_LEFT;
		}
		break;

	case dfACTION_MOVE_LU:
		m_iCurX -= dfSPEED_PLAYER_X;
		m_iCurY -= dfSPEED_PLAYER_Y;

		m_iDirCur = FALSE;

		SetActionMove();

		if (m_iCurX <= dfRANGE_MOVE_LEFT)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurX = dfRANGE_MOVE_LEFT;
		}
		if (m_iCurY <= dfRANGE_MOVE_TOP)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurY = dfRANGE_MOVE_TOP;
		}
		break;
	case dfACTION_MOVE_UU:
		m_iCurY -= dfSPEED_PLAYER_Y;
		SetActionMove();

		if (m_iCurY <= dfRANGE_MOVE_TOP)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurY = dfRANGE_MOVE_TOP;
		}
		break;

	case dfACTION_MOVE_RU:
		m_iCurY -= dfSPEED_PLAYER_Y;
		m_iCurX += dfSPEED_PLAYER_X;

		m_iDirCur = TRUE;
		SetActionMove();

		if (m_iCurX >= dfRANGE_MOVE_RIGHT)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurX = dfRANGE_MOVE_RIGHT;
		}
		if (m_iCurY <= dfRANGE_MOVE_TOP)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurY = dfRANGE_MOVE_TOP;
		}
		break;
	case dfACTION_MOVE_RR:
		m_iCurX += dfSPEED_PLAYER_X;

		m_iDirCur = TRUE;
		SetActionMove();

		if (m_iCurX >= dfRANGE_MOVE_RIGHT)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurX = dfRANGE_MOVE_RIGHT;
		}
		break;

	case dfACTION_MOVE_RD:
		m_iCurY += dfSPEED_PLAYER_Y;
		m_iCurX += dfSPEED_PLAYER_X;

		m_iDirCur = TRUE;
		SetActionMove();

		if (m_iCurX <= dfRANGE_MOVE_LEFT)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurX = dfRANGE_MOVE_LEFT;
		}
		if (m_iCurY >= dfRANGE_MOVE_BOTTOM)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurY = dfRANGE_MOVE_BOTTOM;
		}
		break;
	case dfACTION_MOVE_DD:
		m_iCurY += dfSPEED_PLAYER_Y;
		SetActionMove();

		if (m_iCurY >= dfRANGE_MOVE_BOTTOM)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurY = dfRANGE_MOVE_BOTTOM;
		}
		break;


	case dfACTION_MOVE_LD:
		m_iCurY += dfSPEED_PLAYER_Y;
		m_iCurX -= dfSPEED_PLAYER_X;

		m_iDirCur = FALSE;
		SetActionMove();

		if (m_iCurX >= dfRANGE_MOVE_RIGHT)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurX = dfRANGE_MOVE_RIGHT;
		}
		if (m_iCurY >= dfRANGE_MOVE_BOTTOM)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurY = dfRANGE_MOVE_BOTTOM;
		}
		break;

	case dfACTION_ATTACK1:
		m_dwActionInput = dfACTION_ATTACK1;
		SetActionAttack1();

		break;

	case dfACTION_ATTACK2:
		m_dwActionInput = dfACTION_ATTACK2;
		SetActionAttack2();

		break;

	case dfACTION_ATTACK3:
		m_dwActionInput = dfACTION_ATTACK3;
		SetActionAttack3();

		break;

	case dfACTION_STAND:
		if (m_dwActionInput == dfACTION_ATTACK1 || m_dwActionInput == dfACTION_ATTACK2 || m_dwActionInput == dfACTION_ATTACK3)
			if (!IsEndFrame())
				break;
		SetActionStand();

		break;

	default:
		InputActionProc();
		break;
	}
}

void PlayerObject::Draw(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	g_sprite.DrawSprite50(eSHADOW, m_iCurX, m_iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	g_sprite.DrawSprite(GetSprite(), m_iCurX, m_iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
	g_sprite.DrawSprite(eGUAGE_HP, m_iCurX - 35, m_iCurY + 9, bypDest, iDestWidth, iDestHeight, iDestPitch, m_chHP);
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
	m_dwActionCur = dwAction;
}

void PlayerObject::SetActionAttack1()
{
	if (IsEndFrame()) {
		SetActionStand();
		m_dwActionCur = dfACTION_STAND;
		m_dwActionInput = dfACTION_STAND;
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
		m_dwActionCur = dfACTION_STAND;
		m_dwActionInput = dfACTION_STAND;
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
		m_dwActionCur = dfACTION_STAND;
		m_dwActionInput = dfACTION_STAND;
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

}
