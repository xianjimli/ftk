
#include <string.h>
#include <assert.h>
#include "ftk_wince.h"
#include "ftk_log.h"
#include <initguid.h>

static char g_work_dir[MAX_PATH+1] = {0};
static char g_data_dir[MAX_PATH+1] = {0};
static char g_testdata_dir[MAX_PATH+1] = {0};

char* ftk_get_root_dir(void)
{
	return g_work_dir;
}

char* ftk_get_data_dir(void)
{
	return g_data_dir;
}

char* ftk_get_testdata_dir(void)
{
	return g_testdata_dir;
}

int ftk_platform_init(int argc, char** argv)
{
	int Ret = 0;
	char* p = NULL;
	HRESULT hr;
	WSADATA wsaData = {0};

	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if(FAILED(hr))
	{
		ftk_loge("CoInitializeEx() failed\n");
		return 1;
	}

	if ((Ret = WSAStartup(MAKEWORD(2,2), &wsaData)) != 0)
	{
		assert(!"WSAStartup failed with error %d\n");
		CoUninitialize();
		return 0;
	}
	
	if(_getcwd(g_work_dir, MAX_PATH) != NULL)
	{
		p = strstr(g_work_dir, "\\src");
		if(p != NULL)
		{
			*p = '\0';
			ftk_snprintf(g_data_dir, MAX_PATH, "%s\\data", g_work_dir);
			ftk_snprintf(g_testdata_dir, MAX_PATH, "%s\\testdata", g_work_dir);
		}
	}

	return 0;
}

void ftk_platform_deinit(void)
{
	if (WSACleanup() == SOCKET_ERROR)
	{
		ftk_logd("WSACleanup failed with error %d\n", WSAGetLastError());
	}

	CoUninitialize();
}

char* ftk_strncpy(char *dest, const char *src, size_t n)
{
	return strncpy(dest, src, n);
}

int   ftk_snprintf(char *str, size_t size, const char *format, ...)
{
	int ret = 0;
	va_list args;
	va_start(args, format);
	ret = _vsnprintf(str, size-1, format, args);
	str[size-1] = '\0';

	return ret;
}

int   ftk_vsnprintf(char *str, size_t size, const char *format, va_list ap)
{
	return _vsnprintf(str, size-1, format, ap);
}

size_t ftk_get_relative_time(void)
{
	FILETIME        ft;
	SYSTEMTIME      st;
	LARGE_INTEGER   li;
	__int64         t;

	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);
	li.LowPart  = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	t  = li.QuadPart;       /* In 100-nanosecond intervals */
	t /= 10;                /* In microseconds */

	return (size_t) t/1000;
}

char *_getcwd(char *buf, size_t size)
{
	size_t len;
	len = sizeof("\\Program Files\\ftk") - 1;
	ftk_strncpy(buf, "\\Program Files\\ftk", len);
	buf[len] = '\0';
	return buf;
}

/*http://cantrip.org/socketpair.c*/
/* socketpair.c
 * Copyright 2007 by Nathan C. Myers <ncm@cantrip.org>; all rights reserved.
 * This code is Free Software.  It may be copied freely, in original or 
 * modified form, subject only to the restrictions that (1) the author is
 * relieved from all responsibilities for any use for any purpose, and (2)
 * this copyright notice must be retained, unchanged, in its entirety.  If
 * for any reason the author might be held responsible for any consequences
 * of copying or use, license is withheld.  
 */
int wince_socketpair(SOCKET socks[2])
{
    int e;
    DWORD flags = 0;
    SOCKET listener;
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);

    if (socks == 0) {
      WSASetLastError(WSAEINVAL);
      return SOCKET_ERROR;
    }

    socks[0] = socks[1] = INVALID_SOCKET;
    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) 
        return SOCKET_ERROR;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0x7f000001);
    addr.sin_port = 0;

    e = bind(listener, (const struct sockaddr*) &addr, sizeof(addr));
    if (e == SOCKET_ERROR) {
        e = WSAGetLastError();
    	closesocket(listener);
        WSASetLastError(e);
        return SOCKET_ERROR;
    }

    e = getsockname(listener, (struct sockaddr*) &addr, &addrlen);
    if (e == SOCKET_ERROR) {
        e = WSAGetLastError();
    	closesocket(listener);
        WSASetLastError(e);
        return SOCKET_ERROR;
    }

    do {
        if (listen(listener, 1) == SOCKET_ERROR)                      break;
        if ((socks[0] = socket(AF_INET, SOCK_STREAM, 0))
                == INVALID_SOCKET)                                    break;
        if (connect(socks[0], (const struct sockaddr*) &addr,
                    sizeof(addr)) == SOCKET_ERROR)                    break;
        if ((socks[1] = accept(listener, NULL, NULL))
                == INVALID_SOCKET)                                    break;
        closesocket(listener);
        return 0;
    } while (0);
    e = WSAGetLastError();
    closesocket(listener);
    closesocket(socks[0]);
    closesocket(socks[1]);
    WSASetLastError(e);

    return SOCKET_ERROR;
}

int FTK_MAIN(int argc, char* argv[]);

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev_inst, LPWSTR cmd_link, int cmd_show)
{
    int argc, rv;
    char *argv[2];

    argv[0] = "ftk";
    argv[1] = NULL;
    argc = 1;
    rv = FTK_MAIN(argc, argv);

    return rv;
}
