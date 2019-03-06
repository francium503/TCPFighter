#pragma once

#define dfLOG_LEVEL_DEBUG 0
#define dfLOG_LEVEL_WARNING 1
#define dfLOG_LEVEL_ERROR 2
#define dfLOG_LEVEL_NOTICE 3

void Log(int LogLevel, const WCHAR* fmt, ...);