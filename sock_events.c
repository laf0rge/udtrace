#define _GNU_SOURCE

#include "sock_events.h"
#include "utils.h"

#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef __ANDROID__
#define MUTEX_ERRORCHECK PTHREAD_ERRORCHECK_MUTEX_INITIALIZER
#else
#define MUTEX_ERRORCHECK PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP
#endif

#define IS_OUT	true
#define IS_IN	false

static void trace_data(int fd, bool is_out, const char *fn, const uint8_t *data, unsigned int len)
{
	if (!data || !len)
		return;
	fprintf(stderr, "%d %s %c %s\n", fd, fn, is_out ? 'W' : 'R', udtrace_hexdump(data, len, ""));
}

static void trace_iov(int fd, bool is_out, const char *fn, int ret,
		      const struct iovec *iovec, int iovec_count)
{
	int i, remain;
	if (ret <= 0)
		return;

	remain = ret;
	for (i = 0; i < iovec_count && remain > 0; i++) {
		const struct iovec *vec = &iovec[i];
		int written_len = vec->iov_len;
		if (remain < vec->iov_len)
			written_len = remain;

		trace_data(fd, is_out, fn, vec->iov_base, written_len);

		remain -= written_len;
	}
}

/***********************************************************************
 * Actual socket call handler functions
 ***********************************************************************/

void sock_ev_socket(int fd, int domain, int type, int protocol)
{
	if (domain != AF_UNIX)
		return;
	add_fd(fd);
}

void sock_ev_bind(int fd, int ret, int err, const struct sockaddr *addr, socklen_t len)
{
}

void sock_ev_connect(int fd, int ret, int err, const struct sockaddr *addr,
                     socklen_t len)
{
}

void sock_ev_listen(int fd, int ret, int err, int backlog)
{
}

void sock_ev_accept(int fd, int ret, int err, struct sockaddr *addr,
                    socklen_t *addr_len)
{
	if (ret < 0)
		return;
	add_fd(ret);
}

void sock_ev_accept4(int fd, int ret, int err, struct sockaddr *addr,
                     socklen_t *addr_len, int flags)
{
	if (ret < 0)
		return;
	add_fd(ret);
}

void sock_ev_send(int fd, int ret, int err, const void *buf, size_t bytes,
                  int flags)
{
	if (ret <= 0)
		return;
	trace_data(fd, IS_OUT, "send", buf, ret);
}

void sock_ev_recv(int fd, int ret, int err, void *buf, size_t bytes, int flags)
{
	if (ret <= 0)
		return;
	trace_data(fd, IS_IN, "recv", buf, ret);
}

void sock_ev_sendto(int fd, int ret, int err, const void *buf, size_t bytes,
                    int flags, const struct sockaddr *addr, socklen_t len)
{
	if (ret <= 0)
		return;
	trace_data(fd, IS_OUT, "sendto", buf, ret);
}

void sock_ev_recvfrom(int fd, int ret, int err, void *buf, size_t bytes,
                      int flags, const struct sockaddr *addr, socklen_t *len)
{
	if (ret <= 0)
		return;
	trace_data(fd, IS_IN, "recvfrom", buf, ret);
}

void sock_ev_sendmsg(int fd, int ret, int err, const struct msghdr *msg,
                     int flags)
{
	trace_iov(fd, IS_OUT, "sendmsg", ret, msg->msg_iov, msg->msg_iovlen);
}

void sock_ev_recvmsg(int fd, int ret, int err, const struct msghdr *msg,
                     int flags)
{
	trace_iov(fd, IS_IN, "recvmsg", ret, msg->msg_iov, msg->msg_iovlen);
}

#if !defined(__ANDROID__) || __ANDROID_API__ >= 21
void sock_ev_sendmmsg(int fd, int ret, int err, const struct mmsghdr *vmessages,
                      unsigned int vlen, int flags)
{
	int i;
	if (ret <= 0)
		return;
	for (i = 0; i < vlen; i++) {
		const struct mmsghdr *mm = &vmessages[i];
		trace_iov(fd, IS_OUT, "sendmmsg", mm->msg_len,
			  mm->msg_hdr.msg_iov, mm->msg_hdr.msg_iovlen);
	}
}

void sock_ev_recvmmsg(int fd, int ret, int err, const struct mmsghdr *vmessages,
                      unsigned int vlen, int flags, const struct timespec *tmo)
{

	int i;
	if (ret <= 0)
		return;
	for (i = 0; i < vlen; i++) {
		const struct mmsghdr *mm = &vmessages[i];
		trace_iov(fd, IS_IN, "recvmmsg", mm->msg_len,
			  mm->msg_hdr.msg_iov, mm->msg_hdr.msg_iovlen);
	}
}
#endif

void sock_ev_write(int fd, int ret, int err, const void *buf, size_t bytes)
{
	if (ret <= 0)
		return;
	trace_data(fd, IS_OUT, "write", buf, ret);
}

void sock_ev_read(int fd, int ret, int err, void *buf, size_t bytes)
{
	if (ret <= 0)
		return;
	trace_data(fd, IS_IN, "read", buf, ret);
}

void sock_ev_close(int fd, int ret, int err)
{
	del_fd(fd);
}

void sock_ev_dup(int fd, int ret, int err)
{
	if (ret >= 0)
		add_fd(ret);
}

void sock_ev_dup2(int fd, int ret, int err, int newfd)
{
	if (ret >= 0) {
		del_fd(newfd);
		add_fd(ret);
	}
}

void sock_ev_dup3(int fd, int ret, int err, int newfd, int flags)
{
	if (ret >= 0) {
		del_fd(newfd);
		add_fd(ret);
	}
}

void sock_ev_writev(int fd, int ret, int err, const struct iovec *iovec,
                    int iovec_count)
{
	trace_iov(fd, IS_OUT, "writev", ret, iovec, iovec_count);
}

void sock_ev_readv(int fd, int ret, int err, const struct iovec *iovec,
                   int iovec_count)
{
	trace_iov(fd, IS_IN, "readv", ret, iovec, iovec_count);
}

void sock_ev_sendfile(int fd, int ret, int err, int in_fd, off_t *offset,
                      size_t bytes)
{
	fprintf(stderr, "Cnnot trace sendmsg!");
}
