#include "stdafx.h"
#include "BaseObject.h"


BaseObject::BaseObject()
{
	m_iCurX = 0;
	m_iCurY = 0;
	m_iObjectID = 0;
	m_eObjectType = ObjectType::Player;
	m_iOldX = 0;
	m_iOldY = 0;
	m_dwActionInput = 0;
	m_bEndFrame = 0;
	m_iDelayCount = 0;
	m_iFrameDelay = 0;
	m_iSpriteEnd = 0;
	m_iSpriteNow = 0;
	m_iSpriteStart = 0;
}


BaseObject::~BaseObject()
{
}

void BaseObject::ActionInput(DWORD dwAction)
{
	m_dwActionInput = dwAction;
}

void BaseObject::NextFrame()
{
	if (0 > m_iSpriteStart)
		return;


	m_iDelayCount++;

	if(m_iDelayCount >= m_iFrameDelay)
	{
		m_iDelayCount = 0;

		m_iSpriteNow++;

		if(m_iSpriteNow > m_iSpriteEnd)
		{
			m_iSpriteNow = m_iSpriteStart;
			m_bEndFrame = TRUE;
		}
	}
}

void BaseObject::SetCurPosition(int x, int y)
{
	m_iCurX = x;
	m_iCurY = y;
}

void BaseObject::SetObjectID(int objectID)
{
	m_iObjectID = objectID;
}

void BaseObject::SetObjectType(ObjectType obType)
{
	m_eObjectType = obType;
}

void BaseObject::SetOldPosition(int x, int y)
{
	m_iOldX = x;
	m_iOldY = y;
}

void BaseObject::SetSprite(int spriteStart, int spriteEnd, int frameDelay)
{
	m_iSpriteStart = spriteStart;
	m_iSpriteNow = spriteStart;
	m_iSpriteEnd = spriteEnd;
	m_iFrameDelay = frameDelay;
	m_iDelayCount = 0;
	m_bEndFrame = FALSE;
}
