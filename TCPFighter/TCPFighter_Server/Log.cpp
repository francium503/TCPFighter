#include "pch.h"
#include "Log.h"

int g_LogLevel = dfLOG_LEVEL_DEBUG;  // 나중에 변경 될 수 있게 수정
BOOL g_LogFileWrite = TRUE;

void Log(int LogLevel, const WCHAR* fmt, ...)
{
	if (g_LogLevel <= LogLevel) {
		WCHAR buff[10240];
		va_list arg;
		va_start(arg, fmt);
		wvsprintf(buff, fmt, arg);
		va_end(arg);

		wprintf(L"%s", buff);

		if (g_LogFileWrite == FALSE)
			return;

		FILE *fp = nullptr;
		char fileName[256];

		time_t t = time(0);
		tm now;
		localtime_s(&now, &t);

		sprintf_s(fileName, "%04d_%02d_%02d_Log.txt", now.tm_year + 1900, now.tm_mon + 1, now.tm_mday);

		fopen_s(&fp, fileName, "a+");
		fseek(fp, 0, SEEK_END);

		fwprintf_s(fp, L"%s", buff);

		fclose(fp);
	}
}