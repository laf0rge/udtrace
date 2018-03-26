#pragma once

/*! Determine number of elements in an array of static size */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define LOG(fmt, args ...) \
	fprintf(stderr, ">>> UDTRACE: " fmt, ## args)

/* add a file descriptor from the list of to-be-traced ones */
void udtrace_add_fd(int fd);

/* delete a file descriptor from the list of to-be-traced ones */
void udtrace_del_fd(int fd);

/* set the path of a given fd */
void udtrace_fd_set_path(int fd, const char *path);

/* is the given file descriptor part of the to-be-traced unix domain fd's? */
bool is_unix_socket(int fd);

void udtrace_data(int fd, bool is_out, const char *fn, const uint8_t *data, unsigned int len);
