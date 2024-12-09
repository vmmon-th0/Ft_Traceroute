#include "ft_traceroute.h"

void
sock_send_init ()
{
    g_traceroute.sock_info.send_fd = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (g_traceroute.sock_info.send_fd < 0)
    {
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    g_traceroute.sock_info.addr_4.sin_family = AF_INET;
    g_traceroute.sock_info.addr_4.sin_addr.s_addr = INADDR_ANY;
    g_traceroute.sock_info.addr_4.sin_port = htons (32768 + 666);
}

void
sock_recv_init ()
{
    g_traceroute.sock_info.recv_fd = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (g_traceroute.sock_info.recv_fd < 0)
    {
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;

    if (setsockopt (g_traceroute.sock_info.recv_fd, SOL_SOCKET, SO_RCVTIMEO,
                    &timeout, sizeof (timeout))
        < 0)
    {
        perror ("setsockopt");
        exit (EXIT_FAILURE);
    }
}

void
traceroute_init_g_info ()
{
    bzero (&g_traceroute, sizeof (struct s_traceroute));
    g_traceroute.info.pid = getpid ();
    g_traceroute.info.srcp = (g_traceroute.info.pid & 0xffff) | 0x8000;
}