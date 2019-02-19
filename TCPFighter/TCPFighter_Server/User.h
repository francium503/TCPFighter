#pragma once

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
};

