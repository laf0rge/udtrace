#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"

#define MAX_UNIX_FDS	32

#define LOG(fmt, args ...) \
	fprintf(stderr, ">>> UDTRACE: " fmt, ## args)

/***********************************************************************
 * Utility functions
 ***********************************************************************/

static int unix_fds[MAX_UNIX_FDS];

__attribute__ ((constructor)) static void udtrace_init(void) {
	int i;
	LOG("Unix Domain Socket Trace initialized\n");
	for (i = 0; i < ARRAY_SIZE(unix_fds); i++) {
		unix_fds[i] = -1;
	}
}

/* add a file descriptor from the list of to-be-traced ones */
void add_fd(int fd)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(unix_fds); i++) {
		if (unix_fds[i] == -1) {
			LOG("Adding FD %d\n", fd);
			unix_fds[i] = fd;
			return;
		}
	}
	LOG("Couldn't add UNIX FD %d (no space in unix_fds)\n", fd);
}

/* delete a file descriptor from the list of to-be-traced ones */
void del_fd(int fd)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(unix_fds); i++) {
		if (unix_fds[i] == fd) {
			LOG("Removing FD %d\n", fd);
			unix_fds[i] = -1;
			return;
		}
	}
	LOG("Couldn't delete UNIX FD %d (no such FD)\n", fd);
}

/* is the given file descriptor part of the to-be-traced unix domain fd's? */
bool is_unix_socket(int fd)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(unix_fds); i++) {
		if (unix_fds[i] == fd)
			return true;
	}
	return false;
}


/* taken from libosmocore */
static char hexd_buff[4096];
static const char hex_chars[] = "0123456789abcdef";
char *udtrace_hexdump(const unsigned char *buf, int len, char *delim)
{
	int i;
	char *cur = hexd_buff;

	hexd_buff[0] = 0;
	for (i = 0; i < len; i++) {
		const char *delimp = delim;
		int len_remain = sizeof(hexd_buff) - (cur - hexd_buff);
		if (len_remain < 3)
			break;

		*cur++ = hex_chars[buf[i] >> 4];
		*cur++ = hex_chars[buf[i] & 0xf];

		while (len_remain > 1 && *delimp) {
			*cur++ = *delimp++;
			len_remain--;
		}

		*cur = 0;
	}
	hexd_buff[sizeof(hexd_buff)-1] = 0;
	return hexd_buff;
}
