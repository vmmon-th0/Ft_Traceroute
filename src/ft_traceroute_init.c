#include "ft_traceroute.h"

void
traceroute_socket_send_init()
{
    g_traceroute.sock_info.send_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (g_traceroute.sock_info.send_fd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // fullfil global info
}

void
traceroute_socket_recv_init()
{
    #define TIMEOUT_SEC 1

    g_traceroute.sock_info.recv_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (g_traceroute.sock_info.send_fd < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;

    if (setsockopt(g_traceroute.sock_info.recv_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // fullfil global info
}