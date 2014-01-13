
#ifndef FTK_ANDROID_H
#define FTK_ANDROID_H

#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>

#define ftk_strncpy   strncpy
#define ftk_snprintf  snprintf
#define ftk_vsnprintf vsnprintf
#define ftk_getcwd    getcwd
#define ftk_sscanf    sscanf

#define ftk_pipe_pair(fds)             pipe(fds)
#define ftk_pipe_close(fd)             close(fd)
#define ftk_pipe_recv(fd, buf, length) read(fd, buf, length)
#define ftk_pipe_send(fd, buf, length) write(fd, buf, length)

#ifdef HAVE_CONFIG_H
#include "config_ftk.h"
#endif

#define FTK_PATH_DELIM '/'
#define FTK_HAS_MAIN   1

#endif/*FTK_ANDROID_H*/
