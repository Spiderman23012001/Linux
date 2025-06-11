#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>

// get my ip
int get_my_ip(char *ip_str, size_t max_len);
// connect to peer
int connect_to_peer(const char *ip_str, int port);

#endif // NETWORK_H
