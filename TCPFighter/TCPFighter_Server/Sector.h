#pragma once

class User;

struct st_SECTOR_POS {
	int x;
	int y;
};

struct st_SECTOR_AROUND {
	int Count;
	st_SECTOR_POS Around[9];
};

void SectorAddUser(User* pUser);
void SectorRemoveUser(User* pUser);
BOOL SectorUpdateUser(User* pUser);
void GetSectorAround(int SectorX, int SectorY, st_SECTOR_AROUND *pSectorAround);
void GetUpdateSectorAround(User* pUser, st_SECTOR_AROUND *pRemoveSector, st_SECTOR_AROUND *pAddSector);

