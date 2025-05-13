#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>

int get_my_ip(char *ip_str, size_t max_len);
int connect_to_peer(const char *ip_str, int port);

#endif // NETWORK_H
