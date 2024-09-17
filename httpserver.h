#ifndef HTTPSERVER_H_INCLUDE_
#define HTTPSERVER_H_INCLUDE_

#define _GNU_SOURCE 1

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "ThreadPool.h"
#include "List.h"
#include "Request.h"
#include "Parse.h"
#include "Process.h"
#include "helpers.h"
#include "MutexMap.h"

#define OPTIONS              "t:l:"
#define BUFF_SIZE            4096
#define DEFAULT_THREAD_COUNT 4

extern int errno;

static FILE *logfile;
static int logfile_fd;
static pthread_mutex_t logfile_mutex;

static ThreadPool *tp;
static MutexMap map;

#define LOG(...) fprintf(logfile, __VA_ARGS__);

/**
 * Converts a string to an 16 bits unsigned integer.
 * Returns 0 if the string is malformed or out of the range.
 */
uint16_t strtouint16(char number[]);

/**
 * Creates a socket for listening for connections.
 * Closes the program and prints an error message on error.
 */
int create_listen_socket(uint16_t port);

/** 
 * Handles the connection of an incoming socket connection.
*/
void handle_connection(void *connfd);

#endif
