#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "network.h"


int get_my_ip(char *ip_str, size_t max_len) {
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;
        family = ifa->ifa_addr->sa_family;
        if (family == AF_INET) {
            char host[NI_MAXHOST];
            s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
                            host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
            if (s != 0) {
                fprintf(stderr, "getnameinfo() failed: %s\n", gai_strerror(s));
                continue;
            }
            // Skip loopback interface
            if (strcmp(host, "127.0.0.1") != 0) {
                strncpy(ip_str, host, max_len);
                ip_str[max_len - 1] = '\0';
                freeifaddrs(ifaddr);
                return 0;
            }
        }
    }
    freeifaddrs(ifaddr);
    return -1;
}

int connect_to_peer(const char *ip_str, int port) {
    if (port <= 0 || port > 65535) {
        printf("Error: Invalid port number\n");
        return -1;
    }
    struct sockaddr_in peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_str, &peer_addr.sin_addr) <= 0) {
        printf("Error: Invalid IP address\n");
        return -1;
    }
    // The actual connection logic will be handled in main or connection module
    // This function just validates and prepares the sockaddr_in struct
    return 0;
}
