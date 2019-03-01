#include "pch.h"
#include "User.h"
#include "StreamQ.h"

extern std::list<User *> g_UserList;

User::User(DWORD uID)
{
	m_pRecvQ = new StreamQ(15000);
	m_pSendQ = new StreamQ(15000);
	m_uID = uID;
	m_HP = 100;
	m_X = 10;
	m_Y = 10;
	m_curSector.x = -1;
	m_curSector.y = -1;
	m_oldSector.x = -1;
	m_oldSector.y = -1;
}


User::~User()
{
	delete m_pRecvQ;
	delete m_pSendQ;
	
	closesocket(m_socClient);
}

BOOL User::NetRecvUser()
{
	DWORD lastError;

	int retval = recv(m_socClient, m_pRecvQ->GetRearBufferPtr(), m_pRecvQ->GetNotBrokenPutSize(), 0);

	if (retval == SOCKET_ERROR) {
		lastError = GetLastError();

		if (lastError != WSAEWOULDBLOCK) {
			return FALSE;
		}
		else {
			return TRUE;
		}
	}

	if (retval == 0) {
		// TODO disconnect Ã³¸®
		return FALSE;
	}

	m_pRecvQ->MoveRear(retval);

	return TRUE;
}

BOOL User::NetSendUser()
{
	if (m_pSendQ->GetUseSize() == 0)
		return TRUE;

	DWORD lastError;
	int retval = send(m_socClient, m_pSendQ->GetFrontBufferPtr(), m_pSendQ->GetNotBrokenGetSize(), 0);

	if (retval == SOCKET_ERROR) {
		lastError = GetLastError();

		if (lastError != WSAEWOULDBLOCK) {
			return FALSE;
		}
		else {
			return TRUE;
		}
	}

	m_pSendQ->MoveFront(retval);

	return TRUE;
}