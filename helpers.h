#ifndef HELPERS_H_INCLUDE_
#define HELPERS_H_INCLUDE_

#include "httpserver.h"

// IO helper functions
int writeall(int fd, char *buff, ssize_t len);
int readall(int fd, char *buff, ssize_t len);
int sendall(int connfd, char res[], size_t len);
int write_to_tmpfile(HTTPRequest req, char *buff, ssize_t bytes_read);
int copy_data(int write_fd, char *read_fname, pthread_rwlock_t *mutex);
off_t get_filesize(int fd, struct stat *st);

// HTTPRequest helper functions
int create_and_set_tmpfile(HTTPRequest req);

#endif
