#pragma once
#include "BaseObject.h"
class PlayerObject :
	public BaseObject
{
public:
	PlayerObject(BOOL isPlayer);
	~PlayerObject();


	void Action() override;
	void ActionProc();
	void Draw(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch) override;
	int GetDirection() { return m_iDirCur; }
	BOOL IsPlayer() { return m_bPlayerCharacter; }
	void SetDirection(int direction);
	void SetHP(char hp);
	char GetHP() { return m_chHP; }


protected:
	void SetActionAttack1();
	void SetActionAttack2();
	void SetActionAttack3();
	void SetActionMove();
	void SetActionStand();


	void InputActionProc();



protected:
	BOOL m_bPlayerCharacter;
	char m_chHP;
	DWORD m_dwActionCur;
	DWORD m_dwActionOld;
	int m_iDirCur;
	int m_iDirOld;

};

