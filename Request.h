#ifndef REQUEST_H_INCLUDE_
#define REQUEST_H_INCLUDE_

#include "Headers.h"

// Exported types --------------------------------------------------------------
enum HTTPVerbs_t {
    GET,
    PUT,
    APPEND,
    ERROR,
};
typedef enum HTTPVerbs_t HTTPVerbs_t;
typedef struct HTTPRequestObj *HTTPRequest;

// Constructors-Destructors ---------------------------------------------------
HTTPRequest newHTTPRequest();
void freeHTTPRequest(HTTPRequest *pR);

// Access functions -----------------------------------------------------------
HTTPVerbs_t HTTPRequestGetOp(HTTPRequest R);
Headers HTTPRequestGetHeaders(HTTPRequest R);
char *HTTPRequestGetHeader(HTTPRequest R, char *key);
char *HTTPRequestGetURI(HTTPRequest R);
char *HTTPRequestGetBody(HTTPRequest R);
char *HTTPRequestGetError(HTTPRequest R);
char *HTTPRequestGetTmpFileName(HTTPRequest R);

// Manipulation procedures ----------------------------------------------------
void HTTPRequestClearReq(HTTPRequest R);
void HTTPRequestSetOp(HTTPRequest R, HTTPVerbs_t op);
void HTTPRequestAddHeader(HTTPRequest R, char *key, char *value);
void HTTPRequestUpdateHeader(HTTPRequest R, char *key, char *value);
void HTTPRequestSetURI(HTTPRequest R, char *URI);
void HTTPRequestSetBody(HTTPRequest R, char *body);
void HTTPRequestSetError(HTTPRequest R, char *error);
void HTTPRequestSetTmpFileName(HTTPRequest R, char *tmp_file_name);

// Other Functions ------------------------------------------------------------
void printHTTPRequest(FILE *out, HTTPRequest R);

#endif
