
#ifndef FTK_WINCE_H
#define FTK_WINCE_H

#ifndef WINCE
#include <io.h>
#endif
#include <time.h>
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#ifndef WINCE
#include <sys/stat.h>
#include <sys/types.h>
#endif

#define HAVE_BOOLEAN
#define inline 

char* ftk_get_root_dir(void);
char* ftk_get_data_dir(void);
char* ftk_get_testdata_dir(void);

#define __func__       __FILE__
#define FTK_ROOT_DIR   ftk_get_root_dir()
#define FTK_FONT       "\\Windows\\FZLTHK.TTF"
#define DATA_DIR       ftk_get_root_dir()
#define LOCAL_DATA_DIR ftk_get_root_dir()
#define FTK_DATA_ROOT  ftk_get_data_dir()
#define TESTDATA_DIR   ftk_get_testdata_dir()

#define _CRT_SECURE_NO_WARNINGS 1

#define strcasecmp    _stricmp
#define usleep        Sleep
#define ftk_getcwd    _getcwd
#define ftk_sscanf    sscanf

char* ftk_strncpy(char *dest, const char *src, size_t n);
int   ftk_snprintf(char *str, size_t size, const char *format, ...);
int   ftk_vsnprintf(char *str, size_t size, const char *format, va_list ap);

#ifndef snprintf
#define snprintf ftk_snprintf
#endif

size_t ftk_get_relative_time(void);
char *_getcwd(char *buf, size_t size);
int   wince_socketpair(SOCKET socks[2]);

#define ftk_pipe_pair(fds)				wince_socketpair(fds)
#define ftk_pipe_close(fd)				closesocket(fd)
#define ftk_pipe_recv(fd, buf, length)	recv(fd, buf, length, 0)
#define ftk_pipe_send(fd, buf, length)	send(fd, buf, length, 0)
#define FTK_PATH_DELIM '\\'

#define USE_FTK_MAIN 1

#endif/*FTK_WINCE_H*/
