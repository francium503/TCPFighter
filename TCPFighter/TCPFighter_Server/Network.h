#pragma once

class User;
class PacketBuffer;
struct st_PACKET_HEADER;

BOOL NetPacket_ReqCreateCharacter(User* packUser, PacketBuffer* pPackBuffer);
BOOL NetPacket_ReqMoveStart(User* packUser, PacketBuffer* pPackBuffer);
BOOL NetPacket_ReqMoveStop(User* packUser, PacketBuffer* pPackBuffer);
BOOL NetPacket_ReqAttack1(User* packUser, PacketBuffer* pPackBuffer);
BOOL NetPacket_ReqAttack2(User* packUser, PacketBuffer* pPackBuffer);
BOOL NetPacket_ReqAttack3(User* packUser, PacketBuffer* pPackBuffer);



void Send_CreateCharacter(User* pPacketUser, User* createUser);



void MakePacket_CreateCharacter(st_PACKET_HEADER *packHeader, PacketBuffer* pack, User* createUser);

