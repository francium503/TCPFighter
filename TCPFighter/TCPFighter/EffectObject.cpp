#include "stdafx.h"
#include "EffectObject.h"
#include "Sprite.h"
#include "TCPFighter.h"

extern Sprite g_sprite;

EffectObject::EffectObject()
{
	m_eObjectType = ObjectType::Effect;
	SetSprite(eEFFECT_SPARK_01, eEFFECT_SPARK_04, 3);
}


EffectObject::~EffectObject()
{
}

void EffectObject::Action()
{
	NextFrame();
}

void EffectObject::Draw(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	g_sprite.DrawSprite(GetSprite(), m_iCurX, m_iCurY + 60, bypDest, iDestWidth, iDestHeight, iDestPitch);
}
