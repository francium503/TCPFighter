#pragma once

#include "Sector.h"

class StreamQ;


class User
{
public:
	User(DWORD uID);
	~User();
	
	BOOL NetRecvUser();
	BOOL NetSendUser();


	SOCKET m_socClient;
	StreamQ* m_pRecvQ;
	StreamQ* m_pSendQ;
	DWORD m_uID;
	WORD m_X;
	WORD m_Y;
	BYTE m_HP;
	BYTE m_direction;
	BYTE m_action;
	DWORD m_lastActionTime;
	WORD m_lastActionX;
	WORD m_lastActionY;
	st_SECTOR_POS m_curSector;
	st_SECTOR_POS m_oldSector;
};

