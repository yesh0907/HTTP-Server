#include "httpserver.h"

/**
 * Converts a string to an 16 bits unsigned integer.
 * Returns 0 if the string is malformed or out of the range.
 */
uint16_t strtouint16(char number[]) {
    char *last;
    long num = strtol(number, &last, 10);
    if (num <= 0 || num > UINT16_MAX || *last != '\0') {
        return 0;
    }
    return num;
}

/**
 * Creates a socket for listening for connections.
 * Closes the program and prints an error message on error.
 */
int create_listen_socket(uint16_t port) {
    struct sockaddr_in addr;
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd < 0) {
        err(EXIT_FAILURE, "socket error");
    }

    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(listenfd, (struct sockaddr *) &addr, sizeof addr) < 0) {
        err(EXIT_FAILURE, "bind error");
    }

    if (listen(listenfd, 128) < 0) {
        err(EXIT_FAILURE, "listen error");
    }

    return listenfd;
}

/**
 * helper function to get content length header value
*/
int get_content_length(HTTPRequest req) {
    char *content_length_val = HTTPRequestGetHeader(req, "Content-Length");

    if (content_length_val == NULL) {
        return -1;
    }

    int len = atoi(content_length_val);
    if (len <= 0) {
        return -1;
    }
    return len;
}

/**
 * helper function to get request id header value
*/
unsigned long long get_req_id(HTTPRequest req) {
    char *req_id_header_val = HTTPRequestGetHeader(req, "Request-Id");
    unsigned long long req_id = 0;
    if (req_id_header_val != NULL) {
        req_id = strtoull(req_id_header_val, NULL, 10);
    }
    return req_id;
}

/**
 * returns if the message body has been received
*/
bool received_msg_body(int len, ssize_t total_bytes_read, ssize_t prev_newline_byte) {
    return prev_newline_byte + 1 + len <= total_bytes_read;
}

/**
 * free the c-strings associated with the data in the lines Linked List
*/
void free_lines(List lines) {
    ListMoveFront(lines);

    char *curr_line = NULL;
    while (ListIndex(lines) >= 0) {
        curr_line = ListGet(lines);
        if (curr_line != NULL) {
            free(curr_line);
        }
        ListMoveNext(lines);
    }
}

void read_and_send_file(int connfd, char *res, char *uri, pthread_rwlock_t *mutex) {
    // find end of string
    char *end = strchr(res, '\n');
    if (end != NULL) {
        struct stat st;
        char *fname = uri;
        // ignore initial slash
        fname = fname + 1;

        pthread_rwlock_rdlock(mutex);
        int file_desc = open(fname, O_RDONLY);
        pthread_rwlock_unlock(mutex);
        off_t content_length = get_filesize(file_desc, &st);
        sprintf(end + 1, "Content-Length: %lu\r\n\r\n", content_length);
        // send response status line and headers
        sendall(connfd, res, strlen(res));
        // read and send file
        char file_buffer[BUFF_SIZE] = { 0 };
        ssize_t file_total_bytes_read = 0;
        ssize_t curr_bytes = read(file_desc, file_buffer, BUFF_SIZE);
        while (file_total_bytes_read < content_length && curr_bytes > 0) {
            sendall(connfd, file_buffer, curr_bytes);
            file_total_bytes_read += curr_bytes;
            curr_bytes = read(file_desc, file_buffer, BUFF_SIZE);
        }
        close(file_desc);
    }
}

/** 
 * Handles the connection of an incoming socket connection.
*/
void handle_connection(void *p_connfd) {
    int connfd = *((int *) p_connfd);

    pthread_rwlock_t *file_mutex;
    HTTPRequest req = newHTTPRequest();
    List lines = newList();

    ssize_t curr_buff_size = BUFF_SIZE;
    char *buffer = calloc(curr_buff_size, sizeof(char));
    ssize_t bytes_read = recv(connfd, buffer, curr_buff_size, 0);
    ssize_t total_bytes_read = 0;
    // no prev newline byte found
    ssize_t prev_newline_byte = -1;
    bool headers_terminated = false;
    bool req_parsed = false;
    bool recving_body = false;

    // reused variables
    char *uri;
    int content_length = -1;
    ssize_t offset;

    // read request
    while (bytes_read > 0 && !ThreadPoolStop(tp)) {
        // store request into linked list if we have not processed the request yet
        for (ssize_t i = 0; i < bytes_read && !headers_terminated; i++) {
            if (buffer[total_bytes_read + i] == '\n') {
                ssize_t len = (total_bytes_read + i) - (prev_newline_byte + 1);
                if (len > 0) {
                    char *line = malloc(sizeof(char) * len);
                    if (len > 1) {
                        strncpy(line, buffer + prev_newline_byte + 1, len - 1);
                    }
                    line[len - 1] = '\0';
                    ListAppend(lines, line);
                    prev_newline_byte = total_bytes_read + i;
                }

                // \r\n\r\n pattern found
                if (len == 1) {
                    headers_terminated = true;
                    break;
                }
            }
        }
        total_bytes_read += bytes_read;

        // parse linked list into request
        if (headers_terminated) {
            if (!req_parsed) {
                parseDataToRequest(req, lines);
                uri = HTTPRequestGetURI(req);
                offset = prev_newline_byte + 1;
                content_length = get_content_length(req);
                file_mutex = MutexMapAddEntry(map, uri);
                req_parsed = true;
            }
            HTTPVerbs_t op = HTTPRequestGetOp(req);
            if (op == PUT || op == APPEND) {
                if (received_msg_body(content_length, total_bytes_read, prev_newline_byte)) {
                    if (recving_body) {
                        if (write_to_tmpfile(req, buffer + offset, bytes_read) != 0) {
                            printf("error writing body to tmp file\n");
                        }
                        recving_body = false;
                    } else {
                        if (write_to_tmpfile(req, buffer + offset, content_length) != 0) {
                            printf("error writing body to tmp file\n");
                        }
                    }
                } else {
                    if (offset < total_bytes_read) {
                        recving_body = true;
                        ssize_t bytes_to_write = bytes_read;
                        if (bytes_read > (curr_buff_size - offset)) {
                            bytes_to_write = total_bytes_read - offset;
                        }
                        if (write_to_tmpfile(req, buffer + offset, bytes_to_write) != 0) {
                            printf("error writing body to tmp file\n");
                        }
                    }
                }
            }
            if ((op != PUT && op != APPEND)
                || (!recving_body
                    && received_msg_body(content_length, total_bytes_read, prev_newline_byte))) {
                char res[BUFF_SIZE] = { 0 };
                int *res_status_code = malloc(sizeof(int));
                bool read_write_uri = processRequest(req, res, res_status_code, file_mutex);
                if (strlen(res) > 0) {
                    unsigned long long req_id = get_req_id(req);
                    char *http_op;
                    if (op == GET) {
                        http_op = "GET";
                    } else if (op == PUT) {
                        http_op = "PUT";
                    } else if (op == APPEND) {
                        http_op = "APPEND";
                    } else {
                        http_op = NULL;
                    }

                    if (!read_write_uri) {
                        sendall(connfd, res, strlen(res));
                    } else {
                        if (op == GET) {
                            pthread_mutex_lock(&logfile_mutex);
                            LOG("%s,%s,%d,%llu\n", http_op, uri, *res_status_code, req_id);
                            fflush(logfile);
                            pthread_mutex_unlock(&logfile_mutex);

                            read_and_send_file(connfd, res, uri, file_mutex);
                        }
                    }

                    if (op != GET) {
                        pthread_mutex_lock(&logfile_mutex);
                        LOG("%s,%s,%d,%llu\n", http_op, uri, *res_status_code, req_id);
                        fflush(logfile);
                        pthread_mutex_unlock(&logfile_mutex);
                    }

                    // clean up
                    free(res_status_code);
                    free_lines(lines);
                    ListClear(lines);
                    HTTPRequestClearReq(req);
                    memset(buffer, 0, curr_buff_size);
                    total_bytes_read = 0;
                    prev_newline_byte = -1;
                    headers_terminated = false;
                    req_parsed = false;
                    recving_body = false;
                }
            }
        }

        if (ThreadPoolStop(tp)) {
            break;
        }
        if (recving_body) {
            bytes_read = recv(connfd, buffer + offset, curr_buff_size - offset, 0);
        } else {
            bytes_read
                = recv(connfd, buffer + total_bytes_read, curr_buff_size - total_bytes_read, 0);
        }
    }

    close(connfd);
    free_lines(lines);
    free(buffer);
    freeList(&lines);
    freeHTTPRequest(&req);
    free(p_connfd);
}

static void sigterm_handler(int sig) {
    // free thread pool
    ThreadPoolDestroy(tp);
    // free mutex map
    freeMutexMap(&map);

    if (sig == SIGTERM) {
        warnx("received SIGTERM");
    }
    pthread_mutex_lock(&logfile_mutex);
    fclose(logfile);
    pthread_mutex_unlock(&logfile_mutex);
    // free logfile mutex
    pthread_mutex_destroy(&logfile_mutex);

    exit(EXIT_SUCCESS);
}

static void usage(char *exec) {
    fprintf(stderr, "usage: %s [-t threads] [-l logfile] <port>\n", exec);
}

int main(int argc, char *argv[]) {
    int opt = 0;
    int threads = DEFAULT_THREAD_COUNT;
    logfile = stderr;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 't':
            threads = strtol(optarg, NULL, 10);
            if (threads <= 0) {
                errx(EXIT_FAILURE, "bad number of threads");
            }
            break;
        case 'l':
            logfile = fopen(optarg, "w");
            if (!logfile) {
                errx(EXIT_FAILURE, "bad logfile");
            }
            break;
        default: usage(argv[0]); return EXIT_FAILURE;
        }
    }

    if (optind >= argc) {
        warnx("wrong number of arguments");
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    uint16_t port = strtouint16(argv[optind]);
    if (port == 0) {
        errx(EXIT_FAILURE, "bad port number: %s", argv[1]);
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGTERM, sigterm_handler);
    signal(SIGINT, sigterm_handler);

    // create thread pool
    tp = ThreadPoolCreate(threads);
    if (tp == NULL) {
        errx(EXIT_FAILURE, "could not create thread pool");
    }

    // create mutex mapping for files
    map = newMuxtexMap();
    if (map == NULL) {
        errx(EXIT_FAILURE, "could not create mutex map");
    }

    // get logfile fd
    logfile_fd = fileno(logfile);
    // init logfile mutex
    pthread_mutex_init(&logfile_mutex, NULL);

    int listenfd = create_listen_socket(port);

    for (;;) {
        int connfd = accept(listenfd, NULL, NULL);
        if (connfd < 0) {
            warn("accept error");
            continue;
        }
        int *p_connfd = malloc(sizeof(int));
        *p_connfd = connfd;
        bool worked_added = ThreadPoolAddWork(tp, handle_connection, p_connfd);
        if (!worked_added) {
            warn("did not add work to queue");
            close(connfd);
        }
    }

    return EXIT_SUCCESS;
}
