#pragma once

/*! Determine number of elements in an array of static size */
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/* add a file descriptor from the list of to-be-traced ones */
void add_fd(int fd);

/* delete a file descriptor from the list of to-be-traced ones */
void del_fd(int fd);

/* is the given file descriptor part of the to-be-traced unix domain fd's? */
bool is_unix_socket(int fd);

char *udtrace_hexdump(const unsigned char *buf, int len, char *delim);
