#include <regex.h>
#include <string.h>

#include "Parse.h"

HTTPVerbs_t extractHTTPVerb(char *httpVerb) {
    HTTPVerbs_t verb = ERROR;

    if (strcmp(httpVerb, "GET") == 0) {
        verb = GET;
    } else if (strcmp(httpVerb, "PUT") == 0) {
        verb = PUT;
    } else if (strcmp(httpVerb, "APPEND") == 0) {
        verb = APPEND;
    }

    return verb;
}

bool extractRequestLine(HTTPRequest req, char *line) {
    if (line == NULL) {
        return false;
    }

    regex_t file_name_regex;
    int valid_file_name = regcomp(&file_name_regex, "\\/(\\/?[a-zA-Z0-9_\\.\\-])*", REG_EXTENDED);
    if (valid_file_name != 0) {
        return false;
    }

    char op[8] = { 0 };
    char file[20] = { 0 };
    char protocol[9] = { 0 };

    sscanf(line, "%s %s %s", op, file, protocol);

    bool success = true;
    if (strcmp(op, "GET") == 0 || strcmp(op, "PUT") == 0 || strcmp(op, "APPEND") == 0) {
        HTTPRequestSetOp(req, extractHTTPVerb(op));
    } else {
        HTTPRequestSetError(req, "501: Not Implemented");
        success = false;
    }

    valid_file_name = regexec(&file_name_regex, file, 0, NULL, 0);
    if (valid_file_name == 0) {
        char *filename = strndup(file, 20);
        HTTPRequestSetURI(req, filename);
    } else {
        HTTPRequestSetError(req, "400: Bad Request");
        success = false;
    }

    if (strcmp(protocol, "HTTP/1.1") != 0) {
        HTTPRequestSetError(req, "400: Bad Request");
        success = false;
    }

    if (!success) {
        char *uri = HTTPRequestGetURI(req);
        if (uri != NULL) {
            free(uri);
        }
        HTTPRequestSetOp(req, ERROR);
    }

    // free regex
    regfree(&file_name_regex);

    return success;
}

void extractAndAddHTTPHeaders(HTTPRequest req, List lines) {
    if (req == NULL || lines == NULL) {
        return;
    }

    // move past request line
    ListMoveFront(lines);
    ListMoveNext(lines);

    char *curr_line;
    size_t len;
    bool content_length_added = false;
    bool req_id_added = false;
    while (ListIndex(lines) >= 0) {
        curr_line = ListGet(lines);
        len = strlen(curr_line);
        if (len == 0) {
            return;
        }
        char *colon = strchr(curr_line, ':');
        // no colon or no val after colon
        if (colon == NULL || len - (colon - curr_line) <= 1) {
            HTTPRequestSetOp(req, ERROR);
            HTTPRequestSetError(req, "400: Bad Request");
            break;
        } else {
            char *key = calloc((colon - curr_line) + 1, sizeof(char));
            char *val = calloc((len - (colon - curr_line)), sizeof(char));
            sscanf(curr_line, "%[^:]: %s", key, val);
            if (strcmp(val, "") == 0) {
                free(key);
                free(val);

                HTTPRequestSetOp(req, ERROR);
                HTTPRequestSetError(req, "400: Bad Request");
                break;
            }
            if (strcmp("Content-Length", key) == 0) {
                if (!content_length_added) {
                    HTTPRequestAddHeader(req, key, val);
                    content_length_added = true;
                } else {
                    HTTPRequestUpdateHeader(req, key, val);
                }
            } else if (strcmp("Request-Id", key) == 0) {
                if (!req_id_added) {
                    HTTPRequestAddHeader(req, key, val);
                    req_id_added = true;
                } else {
                    HTTPRequestUpdateHeader(req, key, val);
                }
            } else {
                HTTPRequestAddHeader(req, key, val);
            }
        }

        ListMoveNext(lines);
    }
}

/**
 * Parses the buffered data into HTTPRequest
*/
void parseDataToRequest(HTTPRequest req, List lines) {
    char *request_line = ListFront(lines);
    bool valid_request_line = extractRequestLine(req, request_line);
    if (!valid_request_line) {
        return;
    }
    extractAndAddHTTPHeaders(req, lines);
}
