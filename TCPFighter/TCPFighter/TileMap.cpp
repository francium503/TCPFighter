#include "stdafx.h"
#include "TileMap.h"
#include "TCPFighter.h"
#include "Sprite.h"

extern Sprite g_sprite;

TileMap::TileMap()
{
}


TileMap::~TileMap()
{
}

void TileMap::DrawMap(BYTE * bypDest, int iDestWidth, int iDestHeight, int iDestPitch)
{
	int additionalX = iDrawX % 64;
	int additionalY = iDrawY % 64;

	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 11; ++j) {
			g_sprite.DrawSprite(eSPRITE_TILE, 64 * j - additionalX, 64 * i - additionalY, bypDest, iDestWidth, iDestHeight, iDestPitch);
		}
	}

}

void TileMap::SetDrawX(int iX)
{
	iDrawX = max(dfRANGE_MOVE_LEFT, iX);
	iDrawX = min(iDrawX, dfRANGE_MOVE_RIGHT);
}

void TileMap::SetDrawY(int iY)
{
	iDrawY = max(dfRANGE_MOVE_TOP, iY);
	iDrawY = min(iDrawY, dfRANGE_MOVE_BOTTOM);
}
