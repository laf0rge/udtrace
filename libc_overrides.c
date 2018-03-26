/*
 * (C) 2018 by Harald Welte <laforge@gnumonks.org>
 * based on modified code from tcpsnitch
 * (C) 2016 by Gregory Vander Schueren <gregory.vanderschueren@gmail.com>
 */

#define _GNU_SOURCE

#include <arpa/inet.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "sock_events.h"
#include "utils.h"

#define EXPORT __attribute__((visibility("default")))
#define LIBC_VERSION (__GLIBC__ * 100 + __GLIBC_MINOR__)

#define arg2 a
#define arg3 arg2, b
#define arg4 arg3, c
#define arg5 arg4, d
#define arg6 arg5, e

#define override(FUNCTION, RETURN_TYPE, ARGS_COUNT, ...)                   \
        typedef RETURN_TYPE (*FUNCTION##_type)(int fd, __VA_ARGS__);       \
        FUNCTION##_type orig_##FUNCTION;                                   \
                                                                           \
        EXPORT RETURN_TYPE FUNCTION(int fd, __VA_ARGS__) {                 \
                if (!orig_##FUNCTION)                                      \
                        orig_##FUNCTION =                                  \
                            (FUNCTION##_type)dlsym(RTLD_NEXT, #FUNCTION);  \
                RETURN_TYPE ret = orig_##FUNCTION(fd, arg##ARGS_COUNT);    \
                int err = errno;                                           \
                if (is_unix_socket(fd))                                    \
                        sock_ev_##FUNCTION(fd, ret, err, arg##ARGS_COUNT); \
                errno = err;                                               \
                return ret;                                                \
        }

#define override_1arg(FUNCTION, RETURN_TYPE)                              \
        typedef RETURN_TYPE (*FUNCTION##_type)(int fd);                   \
        FUNCTION##_type orig_##FUNCTION;                                  \
                                                                          \
        EXPORT RETURN_TYPE FUNCTION(int fd) {                             \
                if (!orig_##FUNCTION)                                     \
                        orig_##FUNCTION =                                 \
                            (FUNCTION##_type)dlsym(RTLD_NEXT, #FUNCTION); \
                RETURN_TYPE ret = orig_##FUNCTION(fd);                    \
                int err = errno;                                          \
                if (is_unix_socket(fd)) sock_ev_##FUNCTION(fd, ret, err); \
                errno = err;                                              \
                return ret;                                               \
        }

/*
 Use "standard" font here to generate ASCII arts:
 http://patorjk.com/software/taag/#p=display&f=Standard
*/

/*
  ____   ___   ____ _  _______ _____      _    ____ ___
 / ___| / _ \ / ___| |/ / ____|_   _|    / \  |  _ \_ _|
 \___ \| | | | |   | ' /|  _|   | |     / _ \ | |_) | |
  ___) | |_| | |___| . \| |___  | |    / ___ \|  __/| |
 |____/ \___/ \____|_|\_\_____| |_|   /_/   \_\_|  |___|

 sys/socket.h - Internet Protocol family

 functions: socket(), bind(), connect(), listen(), accept(),
 accept4(), send(), recv(), sendto(), recvfrom(), sendmsg(),
 recvmsg(), sendmmsg(), recvmmsg(),

*/

typedef int (*socket_type)(int domain, int type, int protocol);
socket_type orig_socket;

EXPORT int socket(int domain, int type, int protocol) {
        if (!orig_socket) orig_socket = (socket_type)dlsym(RTLD_NEXT, "socket");
        int fd = orig_socket(domain, type, protocol);
        sock_ev_socket(fd, domain, type, protocol);
        return fd;
}

typedef int (*connect_type)(int fd, const struct sockaddr *addr, socklen_t len);
connect_type orig_connect;

EXPORT int connect(int fd, const struct sockaddr *addr, socklen_t len) {
        if (!orig_connect)
                orig_connect = (connect_type)dlsym(RTLD_NEXT, "connect");

        int ret = orig_connect(fd, addr, len);
        int err = errno;
        if (is_unix_socket(fd)) sock_ev_connect(fd, ret, err, addr, len);

        errno = err;
        return ret;
}

override(bind, int, 3, const struct sockaddr *a, socklen_t b);
override(listen, int, 2, int a);
override(accept, int, 3, struct sockaddr *a, socklen_t *b);
override(accept4, int, 4, struct sockaddr *a, socklen_t *b, int c);

#if defined(__ANDROID__) && __ANDROID_API__ <= 19
override(send, ssize_t, 4, const void *a, size_t b, unsigned int c);
override(recv, ssize_t, 4, void *a, size_t b, unsigned int c);
#else
override(send, ssize_t, 4, const void *a, size_t b, int c);
override(recv, ssize_t, 4, void *a, size_t b, int c);
#endif

override(sendto, ssize_t, 6, const void *a, size_t b, int c,
         const struct sockaddr *d, socklen_t e);
#if defined(__ANDROID__) && __ANDROID_API__ <= 19
override(recvfrom, ssize_t, 6, void *a, size_t b, unsigned int c,
         const struct sockaddr *d, socklen_t *e);
#elif defined(__ANDROID__)
override(recvfrom, ssize_t, 6, void *a, size_t b, int c,
         const struct sockaddr *d, socklen_t *e);
#else
override(recvfrom, ssize_t, 6, void *a, size_t b, int c, struct sockaddr *d,
         socklen_t *e);
#endif

#if defined(__ANDROID__) && __ANDROID_API__ <= 19
override(sendmsg, ssize_t, 3, const struct msghdr *a, unsigned int b);
override(recvmsg, ssize_t, 3, struct msghdr *a, unsigned int b);
#else
override(sendmsg, ssize_t, 3, const struct msghdr *a, int b);
override(recvmsg, ssize_t, 3, struct msghdr *a, int b);
#endif

#if defined(__ANDROID__) && __ANDROID_API__ >= 21
override(sendmmsg, int, 4, const struct mmsghdr *a, unsigned int b, int c);
override(recvmmsg, int, 5, struct mmsghdr *a, unsigned int b, int c,
         const struct timespec *d);
#elif LIBC_VERSION > 219  // Absolutely not sure this is the right boundary!
override(sendmmsg, int, 4, struct mmsghdr *a, unsigned int b, int c);
override(recvmmsg, int, 5, struct mmsghdr *a, unsigned int b, int c,
         struct timespec *d);
#else
override(sendmmsg, int, 4, struct mmsghdr *a, unsigned int b, int c);
override(recvmmsg, int, 5, struct mmsghdr *a, unsigned int b, int c,
         const struct timespec *d);
#endif

/*
  _   _ _   _ ___ ____ _____ ____       _    ____ ___
 | | | | \ | |_ _/ ___|_   _|  _ \     / \  |  _ \_ _|
 | | | |  \| || |\___ \ | | | | | |   / _ \ | |_) | |
 | |_| | |\  || | ___) || | | |_| |  / ___ \|  __/| |
  \___/|_| \_|___|____/ |_| |____/  /_/   \_\_|  |___|

 unistd.h - standard symbolic constants and types

 functions: write(), read(), close(), fork(), dup(), dup2(), dup3()

*/

override(write, ssize_t, 3, const void *a, size_t b);
override(read, ssize_t, 3, void *a, size_t b);

typedef int (*close_type)(int fd);
close_type orig_close;

EXPORT int close(int fd) {
        if (!orig_close) orig_close = (close_type)dlsym(RTLD_NEXT, "close");

        bool is_unix = is_unix_socket(fd);
        int ret = orig_close(fd);
        int err = errno;
        if (is_unix) sock_ev_close(fd, ret, err);

        errno = err;
        return ret;
}

override_1arg(dup, int);
override(dup2, int, 2, int a);
override(dup3, int, 3, int a, int b);

/*
  _   _ _ _____       _    ____ ___
 | | | |_ _/ _ \     / \  |  _ \_ _|
 | | | || | | | |   / _ \ | |_) | |
 | |_| || | |_| |  / ___ \|  __/| |
  \___/|___\___/  /_/   \_\_|  |___|

 sys/uio.h - definitions for vector I/O operations

 functions: writev(), readv()

*/

override(writev, ssize_t, 3, const struct iovec *a, int b);
override(readv, ssize_t, 3, const struct iovec *a, int b);

/*
  ____  _____ _   _ ____  _____ ___ _     _____      _    ____ ___
 / ___|| ____| \ | |  _ \|  ___|_ _| |   | ____|    / \  |  _ \_ _|
 \___ \|  _| |  \| | | | | |_   | || |   |  _|     / _ \ | |_) | |
  ___) | |___| |\  | |_| |  _|  | || |___| |___   / ___ \|  __/| |
 |____/|_____|_| \_|____/|_|   |___|_____|_____| /_/   \_\_|  |___|

 sendfile.h - transfer data between file descriptors

 functions: sendfile()
*/

override(sendfile, ssize_t, 4, int a, off_t *b, size_t c);
