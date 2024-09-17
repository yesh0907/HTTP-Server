#ifndef HEADERS_H_INCLUDE_
#define HEADERS_H_INCLUDE_

#include "List.h"

// Exported type --------------------------------------------------------------
typedef struct HeadersObj *Headers;

// Constructors-Destructors ---------------------------------------------------
Headers newHeaders();
void freeHeaders(Headers *pH);

// Access functions -----------------------------------------------------------
char *HeadersGetHeaderValue(Headers H, char *k);

// Manipulation procedures ----------------------------------------------------
void HeadersAddHeader(Headers H, char *k, char *v);
void HeadersUpdateHeader(Headers H, char *k, char *new_v);
void HeadersRemoveHeader(Headers H, char *k);

// Other functions ------------------------------------------------------------
void printHeaders(FILE *out, Headers H);

#endif
