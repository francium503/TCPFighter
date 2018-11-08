#include "stdafx.h"
#include "PlayerObject.h"
#include "TCPFighter.h"
#include "Sprite.h"

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
		m_iCurX -= 2;

		m_iDirCur = TRUE;
		SetActionMove();

		if (m_iCurX <= dfRANGE_MOVE_LEFT)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurX = dfRANGE_MOVE_LEFT;
		}
		break;

	case dfACTION_MOVE_LU:
	case dfACTION_MOVE_UU:
		m_iCurY -= 3;
		SetActionMove();

		if (m_iCurY <= dfRANGE_MOVE_TOP)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurY = dfRANGE_MOVE_TOP;
		}
		break;

	case dfACTION_MOVE_RU:
	case dfACTION_MOVE_RR:
		m_iCurX += 2;

		m_iDirCur = FALSE;
		SetActionMove();

		if (m_iCurX >= dfRANGE_MOVE_RIGHT)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurX = dfRANGE_MOVE_RIGHT;
		}
		break;

	case dfACTION_MOVE_RD:
	case dfACTION_MOVE_DD:
		m_iCurY += 3;
		SetActionMove();

		if (m_iCurY >= dfRANGE_MOVE_BOTTOM)
		{
			SetActionStand();
			m_dwActionInput = dfACTION_STAND;
			m_iCurY = dfRANGE_MOVE_BOTTOM;
		}
		break;


	case dfACTION_MOVE_LD:

	case dfACTION_ATTACK1:
	case dfACTION_ATTACK2:
	case dfACTION_ATTACK3:
		if(IsEndFrame())
		{
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
	if(m_iDirCur)
		SetSprite(ePLAYER_ATTACK1_L01, ePLAYER_ATTACK1_L04, 3);
	else
		SetSprite(ePLAYER_ATTACK1_R01, ePLAYER_ATTACK1_R04, 3);
}

void PlayerObject::SetActionAttack2()
{
	if (m_iDirCur)
		SetSprite(ePLAYER_ATTACK2_L01, ePLAYER_ATTACK2_L04, 4);
	else
		SetSprite(ePLAYER_ATTACK2_R01, ePLAYER_ATTACK2_R04, 4);
}

void PlayerObject::SetActionAttack3()
{
	if (m_iDirCur)
		SetSprite(ePLAYER_ATTACK3_L01, ePLAYER_ATTACK3_L06, 4);
	else
		SetSprite(ePLAYER_ATTACK3_R01, ePLAYER_ATTACK3_R06, 4);
}

void PlayerObject::SetActionMove()
{
	if (m_iDirCur)
		SetSprite(ePLAYER_MOVE_L01, ePLAYER_MOVE_L12, 4);
	else
		SetSprite(ePLAYER_MOVE_R01, ePLAYER_MOVE_R12, 4);
}

void PlayerObject::SetActionStand()
{
	if(m_iDirCur)
		SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L03, 5);
	else
		SetSprite(ePLAYER_STAND_R01, ePLAYER_STAND_R03, 5);
}

void PlayerObject::InputActionProc()
{

}
