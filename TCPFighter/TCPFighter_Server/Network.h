#pragma once

class User;
class PacketBuffer;

BOOL NetPacket_ReqMoveStart(User* packUser, PacketBuffer* pPackBuffer);
BOOL NetPacket_ReqMoveStop(User* packUser, PacketBuffer* pPackBuffer);
BOOL NetPacket_ReqAttack1(User* packUser, PacketBuffer* pPackBuffer);
BOOL NetPacket_ReqAttack2(User* packUser, PacketBuffer* pPackBuffer);
BOOL NetPacket_ReqAttack3(User* packUser, PacketBuffer* pPackBuffer);


void MakePacket_MoveStart(PacketBuffer* pack, User* moveUser);
void MakePacket_MoveStop(PacketBuffer* pack, User* stopUser);
void MakePacket_Sync(PacketBuffer* pack, User* syncUser);
void MakePacket_CreateMyCharacter(PacketBuffer* pack, User* createUser);
void MakePacket_CreateOtherCharacter(PacketBuffer* pack, User* createUser);
void MakePacket_RemoveCharacter(PacketBuffer* pack, User* deleteUser);


void NewUserJoin(User* pUser);
void UserSectorUpdatePacket(User* pUser);

int DeadReckoningPos(DWORD dwAction, DWORD dwActionTick, int iOldPosX, int iOldPosY, int* pPosX, int* pPosY);
BOOL isHit(User* attacker, User* defender);



void Send_Unicast(User* pClient, PacketBuffer* resPack);
void Send_Sector(PacketBuffer* resPack, int sectorX, int sectorY);
void Send_Around(User* pClient, PacketBuffer* resPack, bool includeClient);
//void Send_All(User* pClient, PacketBuffer* resPack);
