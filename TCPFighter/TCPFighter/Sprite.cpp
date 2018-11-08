#include "stdafx.h"
#include "Sprite.h"

Sprite::Sprite(int iMaxSprtie, DWORD dwColorKey)
{
	m_iMaxSprite = iMaxSprtie;
	m_dwColorKey = dwColorKey;

	m_stpSprite = new st_SPRITE[iMaxSprtie];
}

Sprite::~Sprite()
{
	for(int i=0;i<m_iMaxSprite; ++i)
	{
		ReleaseSprite(i);
	}
}

BOOL Sprite::LoadDibSprite(int iSpriteIndex, WCHAR* szFileName, int iCenterPointX, int iCenterPointY)
{
	FILE* file;
	int iPitch;
	int iImageSize;
	BITMAPFILEHEADER stFileHeader;
	BITMAPINFOHEADER stInfoHeader;

	if (_wfopen_s(&file, szFileName, L"rb") != 0)
		return false;

	
	fseek(file, SEEK_SET, 0);
	if (fread(&stFileHeader, sizeof(stFileHeader), 1, file) != 1) {
		fclose(file);
		return false;
	}

	if(stFileHeader.bfType != 0x4d42) {
		fclose(file);
		return false;
	}

	if (fread(&stInfoHeader, sizeof(stInfoHeader), 1, file) != 1) {
		fclose(file);
		return false;
	}


	int bit = stInfoHeader.biBitCount / 8;

	iPitch = (stInfoHeader.biWidth * bit + 3) & ~3;
	iImageSize = iPitch * stInfoHeader.biHeight;

	m_stpSprite[iSpriteIndex].iWidth = stInfoHeader.biWidth;
	m_stpSprite[iSpriteIndex].iHeight = stInfoHeader.biHeight;
	m_stpSprite[iSpriteIndex].iPitch = iPitch;
	m_stpSprite[iSpriteIndex].iCenterPointX = iCenterPointX;
	m_stpSprite[iSpriteIndex].iCenterPointY = iCenterPointY;

	BYTE *tmp = new BYTE[iImageSize];
	BYTE *spriteTmp = m_stpSprite[iSpriteIndex].bypImage = new BYTE[iImageSize];

	fread(tmp, m_stpSprite[iSpriteIndex].iPitch * stInfoHeader.biHeight, 1, file);

	BYTE *backtmp = tmp + iPitch * (stInfoHeader.biHeight - 1);

	for(int i =0;i<stInfoHeader.biHeight; ++i)
	{
		memcpy(spriteTmp, backtmp, iPitch);
		spriteTmp += iPitch;
		backtmp -= iPitch;
	}


	fclose(file);

	delete[] tmp;

	return true;
}

void Sprite::ReleaseSprite(int iSpriteIndex)
{
	if (m_iMaxSprite <= iSpriteIndex)
		return;

	if (m_stpSprite[iSpriteIndex].bypImage != nullptr)
	{
		delete[] m_stpSprite[iSpriteIndex].bypImage;
	}
}

void Sprite::DrawSprite(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight,
	int iDestPitch, int iDrawLen)
{
	if (iSpriteIndex >= m_iMaxSprite)
		return;

	if (m_stpSprite[iSpriteIndex].bypImage == nullptr)
		return;

	int spriteHeight = m_stpSprite[iSpriteIndex].iHeight;
	int spriteWidth = m_stpSprite[iSpriteIndex].iWidth;
	int spritePitch = m_stpSprite[iSpriteIndex].iPitch;
	BYTE* sprite = m_stpSprite[iSpriteIndex].bypImage;
	int DrawX = iDrawX - m_stpSprite[iSpriteIndex].iCenterPointX;
	int DrawY = iDrawY - m_stpSprite[iSpriteIndex].iCenterPointY;

	if(iDrawX - m_stpSprite[iSpriteIndex].iCenterPointX < 0)
	{
		sprite += -(iDrawX - m_stpSprite[iSpriteIndex].iCenterPointX) * 4;
		spriteWidth -= -(iDrawX - m_stpSprite[iSpriteIndex].iCenterPointX);
		DrawX = 0;
	}

	if(iDrawX - m_stpSprite[iSpriteIndex].iCenterPointX + spriteWidth > iDestWidth)
	{
		spriteWidth -= iDrawX - m_stpSprite[iSpriteIndex].iCenterPointX + spriteWidth - iDestWidth;
	}

	if(iDrawY - m_stpSprite[iSpriteIndex].iCenterPointY < 0)
	{
		sprite += -(iDrawY - m_stpSprite[iSpriteIndex].iCenterPointY) * m_stpSprite[iSpriteIndex].iWidth * 4;
		spriteHeight -= -(iDrawY - m_stpSprite[iSpriteIndex].iCenterPointY);
		DrawY = 0;
	}

	if(iDrawY - m_stpSprite[iSpriteIndex].iCenterPointY + spriteHeight > iDestHeight)
	{
		spriteHeight -= iDrawY - m_stpSprite[iSpriteIndex].iCenterPointY + spriteHeight - iDestHeight;
	}

	if (DrawX > iDestWidth || DrawY > iDestHeight)
		return;

	bypDest += ((DrawY * iDestPitch) + DrawX * 4);

	DWORD* pDest = reinterpret_cast<DWORD*>(bypDest);
	DWORD* pSrc = reinterpret_cast<DWORD*>(sprite);
	BYTE *pDestOrigin = reinterpret_cast<BYTE*>(pDest);
	BYTE *pSpriteOrigin = static_cast<BYTE*>(sprite);

	for (int y = 0; y < spriteHeight; ++y)
	{
		pDest = reinterpret_cast<DWORD*>(pDestOrigin);
		pSrc = static_cast<DWORD*>(pSrc);

		for (int x = 0; x < spriteWidth; ++x)
		{
			if ((*reinterpret_cast<DWORD*>(pSrc) & 0x00ffffff) != m_dwColorKey)
			{
				*pDest = *pSrc;
			}

			pDest++;
			pSrc++;
		}

		pDestOrigin += iDestPitch;
		pSpriteOrigin += spritePitch;
	}
}

void Sprite::DrawSprite50(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight,
	int iDestPitch, int iDrawLen)
{
	if (iSpriteIndex >= m_iMaxSprite)
		return;

	if (m_stpSprite[iSpriteIndex].bypImage == nullptr)
		return;

	int spriteHeight = m_stpSprite[iSpriteIndex].iHeight;
	int spriteWidth = m_stpSprite[iSpriteIndex].iWidth;
	int spritePitch = m_stpSprite[iSpriteIndex].iPitch;
	BYTE* sprite = m_stpSprite[iSpriteIndex].bypImage;
	int DrawX = iDrawX - m_stpSprite[iSpriteIndex].iCenterPointX;
	int DrawY = iDrawY - m_stpSprite[iSpriteIndex].iCenterPointY;

	if (iDrawX - m_stpSprite[iSpriteIndex].iCenterPointX < 0)
	{
		sprite += -(iDrawX - m_stpSprite[iSpriteIndex].iCenterPointX) * 4;
		spriteWidth -= -(iDrawX - m_stpSprite[iSpriteIndex].iCenterPointX);
		DrawX = 0;
	}

	if (iDrawX - m_stpSprite[iSpriteIndex].iCenterPointX + spriteWidth > iDestWidth)
	{
		spriteWidth -= iDrawX - m_stpSprite[iSpriteIndex].iCenterPointX + spriteWidth - iDestWidth;
	}

	if (iDrawY - m_stpSprite[iSpriteIndex].iCenterPointY < 0)
	{
		sprite += -(iDrawY - m_stpSprite[iSpriteIndex].iCenterPointY) * m_stpSprite[iSpriteIndex].iWidth * 4;
		spriteHeight -= -(iDrawY - m_stpSprite[iSpriteIndex].iCenterPointY);
		DrawY = 0;
	}

	if (iDrawY - m_stpSprite[iSpriteIndex].iCenterPointY + spriteHeight > iDestHeight)
	{
		spriteHeight -= iDrawY - m_stpSprite[iSpriteIndex].iCenterPointY + spriteHeight - iDestHeight;
	}

	if (DrawX > iDestWidth || DrawY > iDestHeight)
		return;

	bypDest += ((DrawY * iDestPitch) + DrawX * 4);

	DWORD* pDest = reinterpret_cast<DWORD*>(bypDest);
	DWORD* pSrc = reinterpret_cast<DWORD*>(sprite);
	BYTE *pDestOrigin = reinterpret_cast<BYTE*>(pDest);
	BYTE *pSpriteOrigin = static_cast<BYTE*>(sprite);

	for (int y = 0; y < spriteHeight; ++y)
	{
		pDest = reinterpret_cast<DWORD*>(pDestOrigin);
		pSrc = static_cast<DWORD*>(pSrc);

		for (int x = 0; x < spriteWidth; ++x)
		{
			if ((*reinterpret_cast<DWORD*>(pSrc) & 0x00ffffff) != m_dwColorKey)
			{
				*pDest = *pSrc & 0x7f7f7f7f + *pDest & 0x7f7f7f7f;
			}

			pDest++;
			pSrc++;
		}

		pDestOrigin += iDestPitch;
		pSpriteOrigin += spritePitch;
	}
}

void Sprite::DrawImage(int iSpriteIndex, int iDrawX, int iDrawY, BYTE* bypDest, int iDestWidth, int iDestHeight,
	int iDestPitch, int iDrawLen)
{
	if (iSpriteIndex >= m_iMaxSprite)
		return;

	if (m_stpSprite[iSpriteIndex].bypImage == nullptr)
		return;

	int spriteHeight = m_stpSprite[iSpriteIndex].iHeight;
	int spriteWidth = m_stpSprite[iSpriteIndex].iWidth;
	int spritePitch = m_stpSprite[iSpriteIndex].iPitch;
	BYTE* sprite = m_stpSprite[iSpriteIndex].bypImage;

	bypDest += ((iDrawY * iDestPitch) + iDrawX) * 4;

	DWORD* pDest = reinterpret_cast<DWORD*>(bypDest);
	DWORD* pSrc = reinterpret_cast<DWORD*>(sprite);
	BYTE *pDestOrigin = reinterpret_cast<BYTE*>(pDest);
	BYTE *pSpriteOrigin = static_cast<BYTE*>(sprite);

	for (int y = 0; y < spriteHeight; ++y)
	{
		pDest = reinterpret_cast<DWORD*>(pDestOrigin);
		pSrc = static_cast<DWORD*>(pSrc);

		for (int x = 0; x < spriteWidth; ++x)
		{
			if ((*reinterpret_cast<DWORD*>(sprite) & 0x00ffffff) != m_dwColorKey)
			{
				*pDest = *pSrc;
			}

			pDest++;
			pSrc++;
		}

		pDestOrigin += iDestPitch;
		pSpriteOrigin += spritePitch;
	}
}
