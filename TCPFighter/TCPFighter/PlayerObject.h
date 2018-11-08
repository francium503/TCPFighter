#pragma once
#include "BaseObject.h"
class PlayerObject :
	public BaseObject
{
public:
	PlayerObject();
	~PlayerObject();


	void Action() override;
	void ActionProc();
	void Draw() override;
	int GetDirection();
	BOOL IsPlayer();
	void SetDirection(int direction);
	void SetHP(char hp);



protected:
	void SetActionAttack1();
	void SetActionAttack2();
	void SetActionAttack3();
	void SetActionMove();
	void SetActionStand();



protected:
	BOOL m_bPlayerCharacter;
	char m_chHP;
	DWORD m_dwActionCur;
	DWORD m_dwActionOld;
	int m_iDirCur;
	int m_iDirOld;

};

