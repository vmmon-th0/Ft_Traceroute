#include "ft_traceroute.h"

void
sock_send_init ()
{
    g_traceroute.sock_info.send_fd = socket (AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (g_traceroute.sock_info.send_fd < 0)
    {
        perror ("socket");
        troute_exit (EXIT_FAILURE);
    }

    int optval = 1;
    if (setsockopt (g_traceroute.sock_info.send_fd, IPPROTO_IP, IP_HDRINCL,
                    &optval, sizeof (optval))
        < 0)
    {
        perror ("socket");
        troute_exit (EXIT_FAILURE);
    }

    g_traceroute.sock_info.addr_4.sin_family = AF_INET;
    g_traceroute.sock_info.addr_4.sin_addr.s_addr
        = inet_addr (g_traceroute.sock_info.ip_addr);
    g_traceroute.sock_info.addr_4.sin_port = g_traceroute.info.config.dstp;
}

void
sock_recv_init ()
{
    g_traceroute.sock_info.recv_fd = socket (AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (g_traceroute.sock_info.recv_fd < 0)
    {
        perror ("socket");
        troute_exit (EXIT_FAILURE);
    }
}

void
traceroute_init_g_info ()
{
    bzero (&g_traceroute, sizeof (struct s_traceroute));
    g_traceroute.info.pid = getpid ();
    
    g_traceroute.info.config.max_ttl = 30;
    g_traceroute.info.config.srcp = (g_traceroute.info.pid & 0xffff) | 0x8000;
    g_traceroute.info.config.dstp = 32768 + 666;
    g_traceroute.info.config.nqueries = 3;
}