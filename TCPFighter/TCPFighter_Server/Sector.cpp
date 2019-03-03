#include "pch.h"

#include "Sector.h"
#include "User.h"
#include "Protocol.h"


extern std::list<User *> g_Sector[dfSECTOR_MAX_Y][dfSECTOR_MAX_X];

void SectorAddUser(User * pUser)
{
	if (pUser->m_curSector.x != -1 || pUser->m_curSector.y != -1) {
		return;
	}

	int sectorx = pUser->m_X / dfSECTOR_PIXEL_WIDTH;
	int sectory = pUser->m_Y / dfSECTOR_PIXEL_HEIGHT;

	if (sectorx >= dfSECTOR_MAX_X || sectory >= dfSECTOR_MAX_Y) {
		return;
	}

	g_Sector[sectory][sectorx].push_back(pUser);
	pUser->m_oldSector.x = pUser->m_curSector.x = sectorx;
	pUser->m_oldSector.y = pUser->m_curSector.y = sectory;
}

void SectorRemoveUser(User * pUser)
{
	if (pUser->m_curSector.x == -1 || pUser->m_curSector.y == -1) {
		return;
	}
	
	std::list<User*> &sectorList = g_Sector[pUser->m_curSector.y][pUser->m_curSector.x];

	sectorList.remove(pUser);
	/*
	auto iter = sectorList.begin();

	while (iter != sectorList.end()) {
		if (pUser == *iter) {
			sectorList.erase(iter);
			break;
		}

		++iter;
	}
	*/

	pUser->m_oldSector.x = pUser->m_curSector.x;
	pUser->m_oldSector.y = pUser->m_curSector.y;
	pUser->m_curSector.x = -1;
	pUser->m_curSector.y = -1;
}

BOOL SectorUpdateUser(User * pUser)
{
	int beforeSectorX = pUser->m_curSector.x;
	int beforeSectorY = pUser->m_curSector.y;

	int newSectorx = pUser->m_X / dfSECTOR_PIXEL_WIDTH;
	int newSectory = pUser->m_Y / dfSECTOR_PIXEL_HEIGHT;

	if (beforeSectorX == newSectorx && beforeSectorY == newSectory) {
		return false;
	}

	SectorRemoveUser(pUser);
	SectorAddUser(pUser);

	pUser->m_oldSector.x = beforeSectorX;
	pUser->m_oldSector.y = beforeSectorY;

	/*
	for (int x = 0; x < dfSECTOR_MAX_X; ++x) {
		for (int y = 0; y < dfSECTOR_MAX_Y; ++y) {
			if (g_Sector[y][x].size() != 0) {
				wchar_t buff[256];
				wsprintf(buff, L"sector[%d][%d] - %d\n", y, x, g_Sector[y][x].size());
				OutputDebugString(buff);
			}
		}
	}

	*/

	return true;
}

void GetSectorAround(int SectorX, int SectorY, st_SECTOR_AROUND * pSectorAround)
{
	int cntx;
	int cnty;

	SectorY--;
	SectorX--;

	pSectorAround->Count = 0;

	for (cnty = 0; cnty < 3; ++cnty) {
		if (SectorY + cnty < 0 || SectorY + cnty >= dfSECTOR_MAX_Y) {
			continue;
		}

		for (cntx = 0; cntx < 3; ++cntx) {
			if (SectorX + cntx < 0 || SectorX + cntx >= dfSECTOR_MAX_X) {
				continue;
			}

			pSectorAround->Around[pSectorAround->Count].x = SectorX + cntx;
			pSectorAround->Around[pSectorAround->Count].y = SectorY + cnty;
			pSectorAround->Count++;

		}
	}
}

void GetUpdateSectorAround(User * pUser, st_SECTOR_AROUND * pRemoveSector, st_SECTOR_AROUND * pAddSector)
{
	int cntOld, cntCur;
	bool bFind;
	st_SECTOR_AROUND oldAround, curAround;

	oldAround.Count = 0;
	curAround.Count = 0;


	pRemoveSector->Count = 0;
	pAddSector->Count = 0;

	GetSectorAround(pUser->m_oldSector.x, pUser->m_oldSector.y, &oldAround);
	GetSectorAround(pUser->m_curSector.x, pUser->m_curSector.y, &curAround);

	for (cntOld = 0; cntOld < oldAround.Count; ++cntOld) {
		bFind = false;

		for (cntCur = 0; cntCur < curAround.Count; ++cntCur) {
			if (oldAround.Around[cntOld].x == curAround.Around[cntCur].x && oldAround.Around[cntOld].y == curAround.Around[cntCur].y) {
				bFind = true;
				break;
			}
		}

		if (bFind == false) {
			pRemoveSector->Around[pRemoveSector->Count] = oldAround.Around[cntOld];
			pRemoveSector->Count++;
		}
	}

	for (cntCur = 0; cntCur < curAround.Count; ++cntCur) {
		bFind = false;

		for (cntOld = 0; cntOld < oldAround.Count; ++cntOld) {
			if (oldAround.Around[cntOld].x == curAround.Around[cntCur].x && oldAround.Around[cntOld].y == curAround.Around[cntCur].y) {
				bFind = true;
				break;
			}
		}

		if (bFind == false) {
			pAddSector->Around[pAddSector->Count] = curAround.Around[cntCur];
			pAddSector->Count++;
		}
	}
}

