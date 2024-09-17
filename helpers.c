#include "helpers.h"

int writeall(int fd, char *buff, ssize_t len) {
    ssize_t bytes_written = write(fd, buff, len);
    ssize_t total_bytes_written = 0;

    while (bytes_written > -1) {
        total_bytes_written += bytes_written;
        if (total_bytes_written >= len) {
            return 0;
        }
        bytes_written = write(fd, buff + total_bytes_written, len - total_bytes_written);
    }

    return -1;
}

int readall(int fd, char *buff, ssize_t len) {
    ssize_t bytes_read = read(fd, buff, len);
    ssize_t total_bytes_read = 0;

    while (bytes_read > -1) {
        total_bytes_read += bytes_read;
        if (total_bytes_read >= len) {
            return 0;
        }
        bytes_read = read(fd, buff + total_bytes_read, len - total_bytes_read);
    }

    return -1;
}

int sendall(int connfd, char res[], size_t len) {
    size_t bytes_sent = 0;
    ssize_t curr_bytes = 0;
    while (bytes_sent < len) {
        curr_bytes = send(connfd, res + bytes_sent, len - bytes_sent, 0);
        if (curr_bytes == -1) {
            return -1;
        }
        bytes_sent += curr_bytes;
    }
    return 0;
}

int write_to_tmpfile(HTTPRequest req, char *buff, ssize_t bytes_read) {
    int fd = -1;
    char *tmp_fname = HTTPRequestGetTmpFileName(req);

    if (tmp_fname == NULL) {
        fd = create_and_set_tmpfile(req);
    } else {
        fd = open(tmp_fname, O_RDWR | O_APPEND);
    }
    int bytes_written = writeall(fd, buff, bytes_read);
    close(fd);
    return bytes_written;
}

int copy_data(int write_fd, char *read_fname, pthread_rwlock_t *mutex) {
    if (mutex != NULL) {
        pthread_rwlock_rdlock(mutex);
    }
    int read_fd = open(read_fname, O_RDONLY);
    if (mutex != NULL) {
        pthread_rwlock_unlock(mutex);
    }
    if (read_fd == -1) {
        printf("copy_data: can't open read file\n");
        return -1;
    }

    struct stat read_fd_stat;
    ssize_t file_len = (ssize_t) get_filesize(read_fd, &read_fd_stat);

    char *buffer = calloc(file_len, sizeof(char));
    // read contents of file into buffer
    if (readall(read_fd, buffer, file_len) != 0) {
        printf("copy_data: read error\n");
        free(buffer);
        return -1;
    }

    // write contents of buffer into file
    if (writeall(write_fd, buffer, file_len) != 0) {
        printf("copy_data: write error\n");
        free(buffer);
        return -1;
    }

    free(buffer);
    return 0;
}

off_t get_filesize(int fd, struct stat *st) {
    if (fd == -1) {
        return 0;
    }

    fstat(fd, st);
    return st->st_size;
}

// HTTPRequest helper functions
int create_and_set_tmpfile(HTTPRequest req) {
    char *filename = HTTPRequestGetURI(req);
    // get rid of initial slash
    filename = filename + 1;

    char *tmp_file_name;
    asprintf(&tmp_file_name, "%s-XXXXXX", filename);
    int fd = mkstemp(tmp_file_name);
    HTTPRequestSetTmpFileName(req, tmp_file_name);
    return fd;
}
