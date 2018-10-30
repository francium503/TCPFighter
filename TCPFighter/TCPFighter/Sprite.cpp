#include "stdafx.h"
#include "Sprite.h"
#include <stdio.h>

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
	DWORD dwRead;
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

	m_stpSprite[iSpriteIndex].iWidth = stInfoHeader.biWidth;
	m_stpSprite[iSpriteIndex].iHeight = stInfoHeader.biHeight;
	//m_stpSprite[iSpriteIndex].iPitch = (stInfoHeader.biWidth + 3) & ~3;
	m_stpSprite[iSpriteIndex].iPitch = stInfoHeader.biWidth;
	m_stpSprite[iSpriteIndex].iCenterPointX = iCenterPointX;
	m_stpSprite[iSpriteIndex].iCenterPointY = iCenterPointY;

	int bit = stInfoHeader.biBitCount / 8;
	BYTE *tmp = new BYTE[m_stpSprite[iSpriteIndex].iPitch * stInfoHeader.biHeight * bit];
	m_stpSprite[iSpriteIndex].bypImage = new BYTE[m_stpSprite[iSpriteIndex].iPitch * stInfoHeader.biHeight * bit];

	fread(tmp, m_stpSprite[iSpriteIndex].iPitch * stInfoHeader.biHeight * bit, 1, file);


	for (int i = 0; i < stInfoHeader.biHeight; ++i)
	{
		for (int j = 0; j < m_stpSprite[iSpriteIndex].iPitch * bit; j++)
		{
			m_stpSprite[iSpriteIndex].bypImage[i*m_stpSprite[iSpriteIndex].iPitch*bit + j] = tmp[(stInfoHeader.biHeight - i - 1)*m_stpSprite[iSpriteIndex].iPitch*bit + j];
		}
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

	bypDest += ((DrawY * iDestPitch) + DrawX) * 4;

	for (int y = 0; y < spriteHeight; ++y)
	{
		for (int x = 0; x < spriteWidth; ++x)
		{
			if ((*reinterpret_cast<DWORD*>(sprite) & 0x00ffffff) != m_dwColorKey)
			{
				*(bypDest + 0) = *(sprite + 0);
				*(bypDest + 1) = *(sprite + 1);
				*(bypDest + 2) = *(sprite + 2);
				*(bypDest + 3) = *(sprite + 3);
			}
			sprite += 4;
			bypDest += 4;
		}

		bypDest += (iDestPitch - spriteWidth) * 4;
		sprite += (spritePitch - spriteWidth) * 4;
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

	for (int y = 0; y < spriteHeight; ++y)
	{
		for (int x = 0; x < spritePitch; ++x)
		{
			if ((*reinterpret_cast<DWORD*>(sprite) & 0x00ffffff) != m_dwColorKey)
			{
				*(bypDest + 0) = *(sprite + 0);
				*(bypDest + 1) = *(sprite + 1);
				*(bypDest + 2) = *(sprite + 2);
				*(bypDest + 3) = *(sprite + 3);
			}
			sprite += 4;
			bypDest += 4;
		}

		bypDest += (iDestPitch - spritePitch) * 4;
	}
}
