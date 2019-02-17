#pragma once
class TileMap
{
public:
	TileMap();
	~TileMap();


	void DrawMap(BYTE* bypDest, int iDestWidth, int iDestHeight, int iDestPitch);
	void SetDrawX(int iX);
	void SetDrawY(int iY);
	int GetDrawX() { return iDrawX; }
	int GetDrawY() { return iDrawY; }

private:
	int iDrawX;
	int iDrawY;
};

