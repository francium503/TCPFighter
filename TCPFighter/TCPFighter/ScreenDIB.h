#pragma once
class ScreenDIB
{
public:
	ScreenDIB(int iWidth, int iHeight, int iColorBit);
	virtual ~ScreenDIB();

protected:
	void CreateDibBuffer(int iWidth, int iHeight, int iColorBit);
	void ReleaseDibBuffer();

public:
	void DrawBuffer(HWND hWnd, int iX = 0, int iY = 0);

	BYTE* GetDibBuffer(void);
	int GetWidth(void);
	int GetHeight(void);
	int GetPitch(void);

protected:
	BITMAPINFO m_stDibInfo;
	BYTE *m_bypBuffer;

	int m_iWidth;
	int m_iHeight;
	int m_iPitch;
	int m_iColorBit;
	int m_iBufferSize;
};

