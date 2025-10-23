#ifndef HTTP_H
#define HTTP_H

#include "stdbool.h"

void http_client_ping_init();
bool http_client_get_internet_status();

#endif // HTTP_H