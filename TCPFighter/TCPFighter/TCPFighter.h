#pragma once

#include "resource.h"



#define dfACTION_MOVE_LL 0
#define dfACTION_MOVE_LU 1
#define dfACTION_MOVE_UU 2
#define dfACTION_MOVE_RU 3
#define dfACTION_MOVE_RR 4
#define dfACTION_MOVE_RD 5
#define dfACTION_MOVE_DD 6
#define dfACTION_MOVE_LD 7

#define dfACTION_ATTACK1 10
#define dfACTION_ATTACK2 11
#define dfACTION_ATTACK3 12

#define dfACTION_PUSH
#define dfACTION_STAND 


#define dfDELAY_STAND 5
#define dfDELAY_MOVE 4
#define dfDELAY_ATTACK1 3
#define dfDELAY_ATTACK2 4
#define dfDELAY_ATTACK3 4
#define deDELAY_EFFECT 3


enum class e_SPRITE
{
	ePLAYER_STAND_L01,
	ePLAYER_STAND_L02,
	ePLAYER_STAND_L03,

};