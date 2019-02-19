#include "pch.h"
#include "User.h"
#include "StreamQ.h"

User::User(DWORD uID)
{
	m_pRecvQ = new StreamQ(15000);
	m_pSendQ = new StreamQ(15000);
	m_uID = uID;
	m_HP = 100;
	m_X = 10;
	m_Y = 10;
}


User::~User()
{
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