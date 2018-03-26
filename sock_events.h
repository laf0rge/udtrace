#pragma once

#include <netinet/tcp.h>
#include <pcap/pcap.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <time.h>

// Events hooks

void sock_ev_socket(int fd, int domain, int type, int protocol);

void sock_ev_bind(int fd, int ret, int err, const struct sockaddr *addr,
                  socklen_t len);

void sock_ev_connect(int fd, int ret, int err, const struct sockaddr *addr,
                     socklen_t len);

void sock_ev_listen(int fd, int ret, int err, int backlog);

void sock_ev_accept(int fd, int ret, int err, struct sockaddr *addr,
                    socklen_t *addr_len);

void sock_ev_accept4(int fd, int ret, int err, struct sockaddr *addr,
                     socklen_t *addr_len, int flags);

void sock_ev_send(int fd, int ret, int err, const void *buf, size_t bytes,
                  int flags);

void sock_ev_recv(int fd, int ret, int err, void *buf, size_t bytes, int flags);

void sock_ev_sendto(int fd, int ret, int err, const void *buf, size_t bytes,
                    int flags, const struct sockaddr *addr, socklen_t len);

void sock_ev_recvfrom(int fd, int ret, int err, void *buf, size_t bytes,
                      int flags, const struct sockaddr *addr, socklen_t *len);

void sock_ev_sendmsg(int fd, int ret, int err, const struct msghdr *msg,
                     int flags);

void sock_ev_recvmsg(int fd, int ret, int err, const struct msghdr *msg,
                     int flags);

#if !defined(__ANDROID__) || __ANDROID_API__ >= 21
void sock_ev_sendmmsg(int fd, int ret, int err, const struct mmsghdr *vmessages,
                      unsigned int vlen, int flags);

void sock_ev_recvmmsg(int fd, int ret, int err, const struct mmsghdr *vmessages,
                      unsigned int vlen, int flags, const struct timespec *tmo);
#endif

void sock_ev_write(int fd, int ret, int err, const void *buf, size_t bytes);

void sock_ev_read(int fd, int ret, int err, void *buf, size_t bytes);

void sock_ev_close(int fd, int ret, int err);

void sock_ev_dup(int fd, int ret, int err);

void sock_ev_dup2(int fd, int ret, int err, int newfd);

void sock_ev_dup3(int fd, int ret, int err, int newfd, int flags);

void sock_ev_writev(int fd, int ret, int err, const struct iovec *iovec,
                    int iovec_count);

void sock_ev_readv(int fd, int ret, int err, const struct iovec *iovec,
                   int iovec_count);

void sock_ev_sendfile(int fd, int ret, int err, int in_fd, off_t *offset,
                      size_t bytes);
