#ifndef FT_TRACEROUTE
#define FT_TRACEROUTE

#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <arpa/inet.h>

#ifdef DEBUG
#define TRACEROUTE_DEBUG(fmt, ...)                                                   \
    fprintf (stderr, "DEBUG: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__,  \
             ##__VA_ARGS__)
#else
#define TRACEROUTE_DEBUG(fmt, ...)
#endif

struct s_options
{
    _Bool tcp;
    _Bool icmp;
};

struct s_sock_info
{
    int send_fd;
    int recv_fd;
    struct addrinfo ai;
    struct sockaddr_in addr_4;
    const char *hostname;
    char ai_canonname[NI_MAXHOST];
    char ip_addr[INET_ADDRSTRLEN];
};

struct s_info
{
    bool exit_code;
};

struct s_traceroute
{
    struct s_info info;
    struct s_options options;
    struct s_sock_info sock_info;
};

extern struct s_traceroute g_traceroute;

#endif