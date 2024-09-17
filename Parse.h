#ifndef PARSE_H_INCLUDE_
#define PARSE_H_INCLUDE_

#include "Request.h"

/**
 * Parses the buffered data into HTTPRequest
*/
void parseDataToRequest(HTTPRequest req, List lines);

#endif
