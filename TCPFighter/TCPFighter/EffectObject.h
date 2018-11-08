#pragma once
#include "BaseObject.h"
class EffectObject :
	public BaseObject
{
public:
	EffectObject();
	~EffectObject();
	void Action() override;
	void Draw(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch) override;


protected:
	BOOL m_bEffectStart;
	DWORD m_dwAttackID;
};

