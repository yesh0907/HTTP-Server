#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include "Process.h"
#include "helpers.h"

extern int errno;

/**
 * returns true if there is a file to read 
*/
bool processGet(HTTPRequest req, char res_buffer[], int *res_status_code) {
    char *filename = HTTPRequestGetURI(req);
    // get rid of initial slash
    filename = filename + 1;

    if (access(filename, F_OK) == -1) {
        strcpy(res_buffer, "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n");
        *res_status_code = 404;
    } else {
        strcpy(res_buffer, "HTTP/1.1 200 OK\r\n");
        *res_status_code = 200;
        return true;
    }

    return false;
}

void processPut(HTTPRequest req, char res_buffer[], int *res_status_code, pthread_rwlock_t *mutex) {
    char *filename = HTTPRequestGetURI(req);
    // get rid of initial slash
    filename = filename + 1;

    char *tmp_fname = HTTPRequestGetTmpFileName(req);

    if (tmp_fname == NULL) {
        printf("put write all errno: %s\n", strerror(errno));
        strcpy(res_buffer, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: "
                           "22\r\n\r\nInternal Server Error\n");
        *res_status_code = 500;
    } else {
        pthread_rwlock_wrlock(mutex);
        int rc = unlink(filename);
        if (rc == -1 && errno == ENOENT) {
            strcpy(res_buffer, "HTTP/1.1 201 Created\r\nContent-Length: 8\r\n\r\nCreated\n");
            *res_status_code = 201;
        } else {
            strcpy(res_buffer, "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nOK\n");
            *res_status_code = 200;
        }
        if (rename(tmp_fname, filename) != 0) {
            printf("put rename errno: %s\n", strerror(errno));
        }
        pthread_rwlock_unlock(mutex);
    }
}

void processAppend(
    HTTPRequest req, char res_buffer[], int *res_status_code, pthread_rwlock_t *mutex) {
    char *filename = HTTPRequestGetURI(req);
    // get rid of initial slash
    filename = filename + 1;

    char *append_data_tmp_fname = HTTPRequestGetTmpFileName(req);

    char *tmp_fname;
    int tmp_file_fd = -1;

    bool file_exists = access(filename, F_OK) != -1;

    if (!file_exists) {
        strcpy(res_buffer, "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n");
        *res_status_code = 404;
    } else {
        // create new tmp file data for writing
        tmp_file_fd = create_and_set_tmpfile(req);
        tmp_fname = HTTPRequestGetTmpFileName(req);

        if (tmp_fname == NULL) {
            printf("put write all errno: %s\n", strerror(errno));
            strcpy(res_buffer, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: "
                               "22\r\n\r\nInternal Server Error\n");
            *res_status_code = 500;
        } else {
            strcpy(res_buffer, "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nOK\n");
            *res_status_code = 200;

            copy_data(tmp_file_fd, filename, mutex);
            // close tmp_file_fd and open it with append permissions
            close(tmp_file_fd);
            tmp_file_fd = open(tmp_fname, O_RDWR | O_APPEND);
            // write append data to tmp file
            copy_data(tmp_file_fd, append_data_tmp_fname, NULL);
            close(tmp_file_fd);

            pthread_rwlock_wrlock(mutex);
            if (unlink(filename) != 0) {
                printf("failed to unlink file\n");
                printf("errno: %s\n", strerror(errno));
            }
            if (rename(tmp_fname, filename) != 0) {
                printf("rename errno: %s\n", strerror(errno));
            }
            pthread_rwlock_unlock(mutex);
        }
    }
    unlink(append_data_tmp_fname);
}

bool processRequest(
    HTTPRequest req, char res_buffer[], int *res_status_code, pthread_rwlock_t *mutex) {
    if (req == NULL) {
        return false;
    }

    HTTPVerbs_t op = HTTPRequestGetOp(req);
    bool read_write_uri = false;
    if (op == GET) {
        read_write_uri = processGet(req, res_buffer, res_status_code);
    } else if (op == PUT) {
        processPut(req, res_buffer, res_status_code, mutex);
    } else if (op == APPEND) {
        processAppend(req, res_buffer, res_status_code, mutex);
    }

    return read_write_uri;
}
