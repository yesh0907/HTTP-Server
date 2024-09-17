#include <string.h>

#include "Request.h"

// structs --------------------------------------------------------------------
typedef struct HTTPRequestObj {
    HTTPVerbs_t op;
    Headers headers;
    char *URI;
    char *body;
    char *error;
    char *tmp_file_name;
} HTTPRequestObj;

// Constructors-Destructors ---------------------------------------------------
HTTPRequest newHTTPRequest() {
    HTTPRequest req;
    req = malloc(sizeof(HTTPRequestObj));
    req->op = ERROR;
    req->headers = newHeaders();
    req->body = req->URI = req->error = req->tmp_file_name = NULL;

    return req;
}

void freeHTTPRequest(HTTPRequest *pR) {
    if (pR != NULL && *pR != NULL) {
        HTTPRequest R = *pR;

        freeHeaders(&(R->headers));
        if (R->URI != NULL) {
            free(R->URI);
        }
        if (R->tmp_file_name != NULL) {
            free(R->tmp_file_name);
        }
        free(*pR);
        *pR = NULL;
    }
}

// Access functions -----------------------------------------------------------
HTTPVerbs_t HTTPRequestGetOp(HTTPRequest R) {
    if (R == NULL) {
        fprintf(
            stderr, "Request Error: calling HTTPRequestGetOp() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    return R->op;
}

Headers HTTPRequestGetHeaders(HTTPRequest R) {
    if (R == NULL) {
        fprintf(stderr,
            "Request Error: calling HTTPRequestGetHeaders() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    return R->headers;
}

char *HTTPRequestGetHeader(HTTPRequest R, char *key) {
    if (R == NULL) {
        fprintf(stderr,
            "Request Error: calling HTTPRequestGetHeader() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    return HeadersGetHeaderValue(R->headers, key);
}

char *HTTPRequestGetURI(HTTPRequest R) {
    if (R == NULL) {
        fprintf(
            stderr, "Request Error: calling HTTPRequestGetURI() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    return R->URI;
}

char *HTTPRequestGetBody(HTTPRequest R) {
    if (R == NULL) {
        fprintf(
            stderr, "Request Error: calling HTTPRequestGetBody() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    return R->body;
}

char *HTTPRequestGetError(HTTPRequest R) {
    if (R == NULL) {
        fprintf(
            stderr, "Request Error: calling HTTPRequestGetError() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    return R->error;
}

char *HTTPRequestGetTmpFileName(HTTPRequest R) {
    if (R == NULL) {
        fprintf(stderr,
            "Request Error: calling HTTPRequestGetTmpFileName() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    return R->tmp_file_name;
}

// Manipulation procedures ----------------------------------------------------
void HTTPRequestClearReq(HTTPRequest R) {
    if (R == NULL) {
        fprintf(
            stderr, "Request Error: calling HTTPRequestClearReq() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    freeHeaders(&(R->headers));
    if (R->URI != NULL) {
        free(R->URI);
    }
    if (R->tmp_file_name != NULL) {
        free(R->tmp_file_name);
    }

    R->op = ERROR;
    R->headers = newHeaders();
    R->body = R->URI = R->error = R->tmp_file_name = NULL;
}

void HTTPRequestSetOp(HTTPRequest R, HTTPVerbs_t op) {
    if (R == NULL) {
        fprintf(
            stderr, "Request Error: calling HTTPRequestSetOp() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    R->op = op;
}

void HTTPRequestAddHeader(HTTPRequest R, char *key, char *value) {
    if (R == NULL) {
        fprintf(stderr,
            "Request Error: calling HTTPRequestAddHeader() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    HeadersAddHeader(R->headers, key, value);
}

void HTTPRequestUpdateHeader(HTTPRequest R, char *key, char *value) {
    if (R == NULL) {
        fprintf(stderr,
            "Request Error: calling HTTPRequestUpdateHeader() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    HeadersUpdateHeader(R->headers, key, value);
}

void HTTPRequestSetURI(HTTPRequest R, char *URI) {
    if (R == NULL) {
        fprintf(
            stderr, "Request Error: calling HTTPequestSetURI() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    R->URI = URI;
}

void HTTPRequestSetBody(HTTPRequest R, char *body) {
    if (R == NULL) {
        fprintf(
            stderr, "Request Error: calling HTTPequestSetBody() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    R->body = body;
}

void HTTPRequestSetError(HTTPRequest R, char *error) {
    if (R == NULL) {
        fprintf(
            stderr, "Request Error: calling HTTPequestSetError() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    R->error = error;
}

void HTTPRequestSetTmpFileName(HTTPRequest R, char *tmp_file_name) {
    if (R == NULL) {
        fprintf(
            stderr, "Request Error: calling HTTPequestSetError() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    R->tmp_file_name = tmp_file_name;
}

// Other Functions ------------------------------------------------------------
void printHTTPRequest(FILE *out, HTTPRequest R) {
    if (R == NULL) {
        fprintf(
            stderr, "Request Error: calling printHTTPRequest() on NULL HTTPRequest reference\n");
        exit(EXIT_FAILURE);
    }

    fprintf(out, "HTTP Request Operation: ");
    if (R->op == GET) {
        fprintf(out, "GET");
    } else if (R->op == PUT) {
        fprintf(out, "PUT");
    } else if (R->op == APPEND) {
        fprintf(out, "APPEND");
    } else {
        fprintf(out, "ERROR");
    }
    fprintf(out, "\n");

    if (R->URI != NULL) {
        fprintf(out, "URI: %s\n", R->URI);
    }
    if (R->tmp_file_name != NULL) {
        fprintf(out, "Temp file name: %s\n", R->tmp_file_name);
    }

    fprintf(out, "Headers: ");
    printHeaders(out, R->headers);

    if (R->body != NULL) {
        fprintf(out, "Body: %s\n", R->body);
    }

    if (R->error != NULL) {
        fprintf(out, "Error: %s\n", R->error);
    }
}
