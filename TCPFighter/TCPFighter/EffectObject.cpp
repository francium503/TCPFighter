#include "stdafx.h"
#include "EffectObject.h"
#include "Sprite.h"

extern Sprite g_sprite;

EffectObject::EffectObject()
{
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
	g_sprite.DrawSprite(GetSprite(), m_iCurX, m_iCurY, bypDest, iDestWidth, iDestHeight, iDestPitch);
}
