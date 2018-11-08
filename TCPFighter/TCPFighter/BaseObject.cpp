#include "stdafx.h"
#include "BaseObject.h"


BaseObject::BaseObject()
{
}


BaseObject::~BaseObject()
{
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
	SetOldPosition(m_iCurX, m_iCurY);

	m_iOldX = x;
	m_iOldY = y;
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
