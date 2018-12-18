#pragma once

#include "stdafx.h"
#include "PacketDefine.h"

// S -> C 패킷 처리 함수

BOOL PACKET_SC_CREATE_MY_CHARACTER(char * pack);
BOOL PACKET_SC_CREATE_OTHER_CHARACTER(char * pack);
BOOL PACKET_SC_DELETE_CHARACTER(char * pack);
BOOL PACKET_SC_MOVE_START(char * pack);
BOOL PACKET_SC_MOVE_STOP(char * pack);
BOOL PACKET_SC_ATTACK1(char * pack);
BOOL PACKET_SC_ATTACK2(char * pack);
BOOL PACKET_SC_ATTACK3(char * pack);
BOOL PACKET_SC_DAMAGE(char * pack);


void PACKET_CS_MOVE_START(st_NETWORK_PACKET_HEADER *header, stPACKET_CS_MOVE_START *packet, BYTE direction, WORD x, WORD y);
void PACKET_CS_MOVE_STOP(st_NETWORK_PACKET_HEADER *header, stPACKET_CS_MOVE_STOP *packet, BYTE direction, WORD x, WORD y);
void PACKET_CS_ATTACK1(st_NETWORK_PACKET_HEADER *header, stPACKET_CS_ATTACK1 *packet, BYTE direction, WORD x, WORD y);
void PACKET_CS_ATTACK2(st_NETWORK_PACKET_HEADER *header, stPACKET_CS_ATTACK2 *packet, BYTE direction, WORD x, WORD y);
void PACKET_CS_ATTACK3(st_NETWORK_PACKET_HEADER *header, stPACKET_CS_ATTACK3 *packet, BYTE direction, WORD x, WORD y);