#include "stdafx.h"
#include "ScreenDIB.h"


ScreenDIB::ScreenDIB(int iWidth, int iHeight, int iColorBit)
{
	this->m_iWidth = iWidth;
	this->m_iHeight = iHeight;
	this->m_iColorBit = iColorBit;

	this->m_iPitch = (m_iWidth * iColorBit / 8 + 3) & ~3;
	this->m_iBufferSize = m_iHeight * m_iPitch;

	CreateDibBuffer(iWidth, iHeight, iColorBit);
}


ScreenDIB::~ScreenDIB()
{
	ReleaseDibBuffer();
}

void ScreenDIB::CreateDibBuffer(int iWidth, int iHeight, int iColorBit)
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_iColorBit = iColorBit;

	m_iPitch = (m_iWidth * iColorBit / 8 + 3) & ~3;
	m_iBufferSize = m_iHeight * m_iPitch;

	m_stDibInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_stDibInfo.bmiHeader.biPlanes = 1;
	m_stDibInfo.bmiHeader.biHeight = -iHeight;
	m_stDibInfo.bmiHeader.biWidth = iWidth;
	m_stDibInfo.bmiHeader.biBitCount = iColorBit;

	m_bypBuffer = new BYTE[m_iBufferSize];
	memset(m_bypBuffer, 0xff, m_iBufferSize);

}

void ScreenDIB::ReleaseDibBuffer()
{
	if (m_bypBuffer != nullptr) {
		delete[] m_bypBuffer;

		m_bypBuffer = nullptr;
	}
}

void ScreenDIB::DrawBuffer(HWND hWnd, int iX, int iY)
{
	HDC hdc = GetDC(hWnd);

	StretchDIBits(hdc, iX, iY, m_iWidth, m_iHeight, 0, 0, m_iWidth, m_iHeight, m_bypBuffer, &m_stDibInfo, DIB_RGB_COLORS, SRCCOPY);

	ReleaseDC(hWnd, hdc);
}

BYTE* ScreenDIB::GetDibBuffer()
{
	return m_bypBuffer;
}

int ScreenDIB::GetWidth()
{
	return m_iWidth;
}

int ScreenDIB::GetHeight()
{
	return m_iHeight;
}

int ScreenDIB::GetPitch()
{
	return m_iPitch;
}
