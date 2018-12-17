#pragma once

#include "stdafx.h"


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
