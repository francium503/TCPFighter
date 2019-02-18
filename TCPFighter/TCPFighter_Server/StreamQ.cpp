#include "pch.h"
#include "StreamQ.h"

StreamQ::StreamQ(void)
{
	m_pBuffer = new char[eBuffer_DEFAULT];
	m_front = 0;
	m_rear = 1;
	m_bufferSize = eBuffer_DEFAULT;
}

StreamQ::StreamQ(int iBufferSize)
{
	m_pBuffer = new char[iBufferSize];
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

	m_pBuffer = new char[iSize];

	m_front = 0;
	m_rear = 0;
	m_bufferSize = iSize;
}

int StreamQ::GetBufferSize(void)
{
	return m_bufferSize;
}

int StreamQ::GetUseSize(void)
{
	if (m_rear < m_front) {
		return (m_rear)+(m_bufferSize - (m_front + 1));
	}
	else {
		return m_rear - m_front - 1;
	}
}

int StreamQ::GetFreeSize(void)
{
	if (m_rear < m_front) {
		return m_front - m_rear - 1;
	}
	else {
		return (m_front - 1) + (m_bufferSize - m_rear - 1);
	}
}

int StreamQ::GetNotBrokenGetSize(void)
{
	if (m_rear > m_front) {
		return m_rear - m_front - 1;
	}
	else {
		if (m_front + 1 == m_bufferSize) {
			return m_rear;
		}
		else {
			return m_bufferSize - ((m_front + 1) % m_bufferSize);
		}
	}
}

int StreamQ::GetNotBrokenPutSize(void)
{
	if (m_rear > m_front) {
		return m_bufferSize - m_rear;
	}
	else {
		return m_front - m_rear;
	}
}

int StreamQ::Enqueue(char * chpData, int iSize)
{
	int putSize = GetNotBrokenPutSize();
	char* pRear = &m_pBuffer[(m_rear) % m_bufferSize];


	if (putSize > iSize) {
		memcpy_s(pRear, iSize, chpData, iSize);
		MoveRear(iSize);

		return iSize;
	}
	else {
		memcpy_s(pRear, putSize, chpData, putSize);
		MoveRear(putSize);

		int dataSize = iSize - putSize;
		pRear = &m_pBuffer[(m_rear) % m_bufferSize];

		putSize = GetNotBrokenPutSize();

		if (putSize >= dataSize) {
			memcpy_s(pRear, dataSize, &chpData[iSize - dataSize], dataSize);
			MoveRear(dataSize);

			return iSize;
		}
		else {
			memcpy_s(pRear, putSize, &chpData[iSize - dataSize], putSize);
			MoveRear(putSize);

			return iSize - dataSize + putSize;
		}
	}
}

int StreamQ::DeQueue(char * chpDest, int iSize)
{
	int getSize = GetNotBrokenGetSize();
	char* pFront = &m_pBuffer[(m_front + 1) % m_bufferSize];

	if (getSize > iSize) {
		memcpy_s(chpDest, iSize, pFront, iSize);
		MoveFront(iSize);

		return iSize;
	}
	else {
		memcpy_s(chpDest, getSize, pFront, getSize);
		MoveFront(getSize);

		int dataSize = iSize - getSize;

		pFront = &m_pBuffer[(m_front + 1) % m_bufferSize];

		getSize = GetNotBrokenGetSize();

		if (getSize > dataSize) {
			memcpy_s(&chpDest[iSize - dataSize], dataSize, pFront, dataSize);
			MoveFront(dataSize);

			return iSize;
		}
		else {
			memcpy_s(&chpDest[iSize - dataSize], getSize, pFront, getSize);
			MoveFront(getSize);

			return iSize - dataSize + getSize;
		}
	}
}

int StreamQ::Peek(char * chpDest, int iSize)
{
	int getSize = GetNotBrokenGetSize();
	char* pFront = &m_pBuffer[(m_front + 1) % m_bufferSize];
	int tmpFront = m_front;

	if (getSize > iSize) {
		memcpy_s(chpDest, iSize, pFront, iSize);

		return iSize;
	}
	else {
		memcpy_s(chpDest, getSize, pFront, getSize);
		MoveFront(getSize);

		int dataSize = iSize - getSize;

		pFront = &m_pBuffer[(m_front + 1) % m_bufferSize];

		getSize = GetNotBrokenGetSize();

		if (getSize > dataSize) {
			memcpy_s(&chpDest[iSize - dataSize], dataSize, pFront, dataSize);
			MoveFront(dataSize);
			m_front = tmpFront;

			return iSize;
		}
		else {
			memcpy_s(&chpDest[iSize - dataSize], getSize, pFront, getSize);
			MoveFront(getSize);
			m_front = tmpFront;

			return iSize - dataSize + getSize;
		}
	}
}

int StreamQ::MoveRear(int iSize)
{
	m_rear = (m_rear + iSize) % m_bufferSize;

	return iSize;
}

int StreamQ::MoveFront(int iSize)
{
	m_front = (m_front + iSize) % m_bufferSize;

	return iSize;
}

void StreamQ::ClearBuffer(void)
{
	m_front = 0;
	m_rear = 1;
}

char * StreamQ::GetFrontBufferPtr(void)
{
	return &m_pBuffer[(m_front + 1) % m_bufferSize];
}

char * StreamQ::GetRearBufferPtr(void)
{
	return &m_pBuffer[(m_rear) % m_bufferSize];
}
