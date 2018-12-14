#pragma once

/*! Determine number of elements in an array of static size */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define LOG(fmt, args ...) \
	fprintf(stderr, ">>> UDTRACE: " fmt, ## args)

typedef void (*udtrace_dissector)(int fd, bool is_out, const char *fn,
	const uint8_t *data, unsigned int len);

struct sock_state {
	int fd;
	const char *path;
	udtrace_dissector dissector;
};

/* find the state corresponding to a given file descriptor */
struct sock_state *udtrace_sstate_by_fd(int fd);

/* add a file descriptor from the list of to-be-traced ones */
void udtrace_add_fd(int fd);

/* add a file descriptor from the list of to-be-traced ones */
void udtrace_add_fd_child(int pfd, int cfd);

/* delete a file descriptor from the list of to-be-traced ones */
void udtrace_del_fd(int fd);

/* set the path of a given fd */
void udtrace_fd_set_path(int fd, const char *path);

/* is the given file descriptor part of the to-be-traced unix domain fd's? */
bool is_unix_socket(int fd);

void udtrace_data(int fd, bool is_out, const char *fn, const uint8_t *data, unsigned int len);
