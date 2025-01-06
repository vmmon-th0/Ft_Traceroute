#ifndef FT_TRACEROUTE
#define FT_TRACEROUTE

#include <arpa/inet.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#ifdef DEBUG
#define TRACEROUTE_DEBUG(fmt, ...)                                             \
    fprintf (stderr, "DEBUG: %s:%d:%s(): " fmt, __FILE__, __LINE__, __func__,  \
             ##__VA_ARGS__)
#else
#define TRACEROUTE_DEBUG(fmt, ...)
#endif

#define BUFFER_SIZE 1500

struct s_config
{
    uint8_t  nqueries;
    uint8_t  max_ttl;
    uint16_t port;
    uint16_t srcp;
    uint16_t dstp;
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

struct s_rtt
{
    struct timespec start;
    struct timespec end;
};

struct s_info
{
    pid_t pid;
    _Bool got_alarm;
    struct sockaddr_in last_sa;
    struct s_rtt rtt_metrics;
    struct s_config config;
};

struct s_troute_pkt
{
    struct iphdr iphdr;
    struct udphdr udphdr;
};

struct s_traceroute
{
    struct s_info info;
    struct s_sock_info sock_info;
};

void traceroute_coord (const char *hostname);
void traceroute_init_g_info ();
void fill_troute_packet (struct s_troute_pkt *troute_pkt, int ttl);
void sock_send_init ();
void sock_recv_init ();
void troute_exit (int status);
void fqdn_resolver (const char *ip, char *fqdn, size_t fqdn_size);
void resolve_hostname (const char *hostname);
double compute_elapsed_ms (struct timespec start, struct timespec end);
_Bool verify_checksum (struct icmphdr *icmphdr, size_t pkt_size);

extern struct s_traceroute g_traceroute;

#endif