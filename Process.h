#ifndef PROCESS_H_INCLUDE_
#define PROCESS_H_INCLUDE_

#include <stdbool.h>
#include <pthread.h>
#include "Request.h"

/**
 * Process the HTTP Request, stores the appropriate
 * server response in provided buffer, and returns
 * true or false if the URI file contents needs to
 * sent
*/
bool processRequest(
    HTTPRequest req, char res_buffer[], int *res_status_code, pthread_rwlock_t *mutex);

#endif
