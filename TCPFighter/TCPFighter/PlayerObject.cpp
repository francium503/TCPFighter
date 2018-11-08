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

void PlayerObject::SetActionStand()
{
	SetSprite(ePLAYER_STAND_L01, ePLAYER_STAND_L03, 5);
}

void PlayerObject::InputActionProc()
{

}
