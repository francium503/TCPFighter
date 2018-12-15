#include "stdafx.h"
#include "StreamQ.h"
#include <iostream>

StreamQ::StreamQ(void) {
	m_pBuffer = nullptr;
	m_front = 0;
	m_rear = 0;
	m_bufferSize = 0;
}

StreamQ::StreamQ(int iBufferSize) {
	m_pBuffer = new char[iBufferSize];

#ifdef __StreamQDebug__
	for (int i = 0; i < iBufferSize; ++i)
		m_pBuffer[i] = '\0';
#endif

	m_front = 0;
	m_rear = 1;
	m_bufferSize = iBufferSize;
}

StreamQ::~StreamQ()
{
	delete[] m_pBuffer;
}

void StreamQ::ReSize(int iSize)
{
	delete[] m_pBuffer;

	m_bufferSize = iSize;
	m_pBuffer = new char[iSize];
	m_front = 0;
	m_rear = 1;
}

int StreamQ::GetBufferSize(void)
{
	return m_bufferSize;
}

int StreamQ::GetUseSize(void)
{
	if (m_rear > m_front) {
		return m_rear - m_front - 1;
	}
	else {
		return m_bufferSize - m_front + m_rear - 1;
	}
}

int StreamQ::GetFreeSize(void)
{
	if (m_rear > m_front) {
		return m_front + m_bufferSize - m_rear - 1;
	}
	else {
		return m_front - m_rear;
	}
}

int StreamQ::GetNotBrokenGetSize(void)
{
	if (m_front < m_rear)
		return m_rear - (m_front + 1);
	else
		return m_bufferSize - (m_front + 1);
}

int StreamQ::GetNotBrokenPutSize(void)
{
	if (m_rear > m_front)
		return m_bufferSize - m_rear;
	else
		return m_front - m_rear - 1;
}

int StreamQ::Enqueue(char * chpData, int iSize)
{
	if (GetFreeSize() == 0)
		return 0;


	int notBrokenSize = GetNotBrokenPutSize();
	if (notBrokenSize >= iSize) {
		memcpy_s(&m_pBuffer[m_rear], notBrokenSize, chpData, iSize);

		m_rear = (m_rear + iSize) % m_bufferSize;

		return iSize;
	}
	else {
		memcpy_s(&m_pBuffer[m_rear], notBrokenSize, chpData, notBrokenSize);
		if (m_front != 0) {
			if (m_front - 1 > iSize - notBrokenSize) {
				memcpy_s(m_pBuffer, iSize - notBrokenSize, &chpData[notBrokenSize], iSize - notBrokenSize);

				m_rear = iSize - notBrokenSize;
				return iSize;
			}
			else {
				memcpy_s(m_pBuffer, m_front - 1, &chpData[notBrokenSize], m_front - 1);

				m_rear = m_front - 1;
				return notBrokenSize + m_rear;
			}
		}
		else {
			m_rear = m_rear + notBrokenSize;
			return notBrokenSize;
		}
	}
}

int StreamQ::DeQueue(char * chpDest, int iSize)
{
	if (GetUseSize() == 0)
		return 0;

	int notBrokenSize = GetNotBrokenGetSize();
	if (notBrokenSize >= iSize) {
		memcpy_s(chpDest, iSize, &m_pBuffer[m_front + 1], iSize);

		m_front = (m_front + iSize) % m_bufferSize;
		return iSize;
	}
	else {
		memcpy_s(chpDest, notBrokenSize, &m_pBuffer[m_front + 1], notBrokenSize);

		if (m_rear == 0) {
			m_front = m_bufferSize;
			return notBrokenSize;
		}
		else {
			if (m_rear > iSize - notBrokenSize) {
				memcpy_s(&chpDest[notBrokenSize], iSize - notBrokenSize, m_pBuffer, iSize - notBrokenSize);


				m_front = iSize - notBrokenSize;
				return iSize;
			}
			else {
				memcpy_s(&chpDest[notBrokenSize], m_rear, m_pBuffer, m_rear);

				m_front = m_rear - 1;
				return notBrokenSize + m_rear;
			}
		}
	}
}

int StreamQ::Peek(char * chpDest, int iSize)
{
	if (GetUseSize() == 0)
		return 0;

	int notBrokenSize = GetNotBrokenGetSize();
	if (notBrokenSize >= iSize) {
		memcpy_s(chpDest, iSize, &m_pBuffer[m_front + 1], iSize);

		return iSize;
	}
	else {
		memcpy_s(chpDest, notBrokenSize, &m_pBuffer[m_front + 1], notBrokenSize);

		if (m_rear == 0) {
			return notBrokenSize;
		}
		else {
			if (m_rear > iSize - notBrokenSize) {
				memcpy_s(&chpDest[notBrokenSize], iSize - notBrokenSize, m_pBuffer, iSize - notBrokenSize);

				return iSize;
			}
			else {
				memcpy_s(&chpDest[notBrokenSize], m_rear, m_pBuffer, m_rear);

				return notBrokenSize + m_rear;
			}
		}
	}
}

int StreamQ::MoveFront(int iSize)
{
	if ((m_front + iSize) % m_bufferSize < m_rear) {
		m_front = (m_front + iSize) % m_bufferSize;
		return iSize;
	}
	else {
		int tmp = (m_front + iSize) % m_bufferSize;
		m_front = m_rear - 1;

		return tmp - m_front;
	}
}

void StreamQ::ClearBuffer(void)
{
	m_front = 0;
	m_rear = 1;
}

char * StreamQ::GetFrontBufferPtr(void)
{
	return &m_pBuffer[m_front + 1];
}

char * StreamQ::GetRearBufferPtr(void)
{
	return &m_pBuffer[m_rear];
}
