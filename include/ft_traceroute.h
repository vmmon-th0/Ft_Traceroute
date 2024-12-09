#ifndef FT_TRACEROUTE
#define FT_TRACEROUTE

#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#ifdef DEBUG
#define TRACEROUTE_DEBUG(fmt, ...)                                             \
    fprintf (stderr, "DEBUG: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__,  \
             ##__VA_ARGS__)
#else
#define TRACEROUTE_DEBUG(fmt, ...)
#endif

#define TIMEOUT_SEC 1

#define PACKET_SIZE 64
#define UDP_PAYLOAD_SIZE                                                       \
    PACKET_SIZE - sizeof (struct iphdr) - sizeof (struct udphdr)

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
    pid_t pid;
    uint16_t nsent;
    uint16_t srcp, dstp;
    uint16_t ttl, max_ttl;
    uint16_t probe, nprobes;
    _Bool exit_code;
    _Bool ready_send;
};

struct s_udp_pkt
{
    struct udphdr udphdr;
    char data[UDP_PAYLOAD_SIZE];
};

struct s_traceroute
{
    struct s_info info;
    struct s_options options;
    struct s_sock_info sock_info;
};

void release_resources ();
void traceroute_coord (const char *hostname);
void traceroute_init_g_info ();
void fill_udp_packet (struct s_udp_pkt *udp_pkt);
void sock_send_init ();
void sock_recv_init ();

extern struct s_traceroute g_traceroute;

#endif