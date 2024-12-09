#include "ft_traceroute.h"

/**
 * @brief This function resolves a hostname that will be the traceroute
 * destination target, we are dealing with IPV4, The getaddrinfo() function
 * allocates and initializes a linked list of addrinfo structures. There are
 * several reasons why the linked list may have more than one addrinfo structure
 * :
 * - Multihoming (A network host can be reached via multiple IP addresses)
 * - Multiple protocols (AF_INET (IPv4) and AF_INET6 (IPv6))
 * - Various socket types (The same service can be reached via different socket
 * types, such as SOCK_STREAM (TCP) and SOCK_DGRAM (UDP))
 * @param hostname destination hostname
 */

static void
resolve_hostname (const char *hostname)
{
    struct addrinfo hints, *res, *p;
    int status;

    memset (&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_flags = AI_CANONNAME;

    if ((status = getaddrinfo (hostname, NULL, &hints, &res)) != 0)
    {
        fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (status));
        exit (EXIT_FAILURE);
    }

    for (p = res; p != NULL; p = p->ai_next)
    {
        if (p->ai_family == AF_INET)
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            void *addr = &(ipv4->sin_addr);

            if (inet_ntop (p->ai_family, addr, g_traceroute.sock_info.ip_addr,
                           sizeof (g_traceroute.sock_info.ip_addr))
                == NULL)
            {
                perror ("inet_ntop IPv4");
                continue;
            }
            memcpy (&g_traceroute.sock_info.ai, p, sizeof (struct addrinfo));
            TRACEROUTE_DEBUG ("IPv4 addr for %s: %s\n", hostname,
            g_traceroute.sock_info.ip_addr);
            break;
        }
    }

    if (res->ai_canonname)
    {
        TRACEROUTE_DEBUG ("Canonname for IPv4 addr %s: %s\n",
        g_traceroute.sock_info.ip_addr, res->ai_canonname);
        memcpy (g_traceroute.sock_info.ai_canonname, res->ai_canonname,
                sizeof (g_traceroute.sock_info.ai_canonname));
        g_traceroute.sock_info.ai.ai_canonname
            = g_traceroute.sock_info.ai_canonname;
    }

    freeaddrinfo (res);

    if (p == NULL)
    {
        fprintf (stderr, "No valid IPv4 address found for %s\n", hostname);
        exit (EXIT_FAILURE);
    }

    g_traceroute.sock_info.hostname = hostname;
}

void
traceroute_coord (const char *hostname)
{
    resolve_hostname (hostname);

    sock_send_init ();
    sock_recv_init ();

    // Implement loop for tracerouting

    g_traceroute.info.ttl = 1;
    g_traceroute.info.max_ttl = 30;

    for (int ttl = g_traceroute.info.ttl; ttl <= g_traceroute.info.max_ttl;
         ++ttl)
    {
        // Notes : for each TTL, we send three probe packets.
        // Initial dest port is 32768 + 666, which will be incremented by one
        // each time we send a UDP datagram. (WE hope that theses ports are not
        // in use on the dest host) SEND RECV

        // At each loop we use setsockopt to increase ttl size

        if (setsockopt (g_traceroute.sock_info.send_fd, IPPROTO_IP, IP_TTL,
                        &ttl, sizeof (ttl)) < 0)
        {
            perror ("setsockopt");
            release_resources ();
            exit (EXIT_FAILURE);
        }

        // fullfill packet here

        struct s_udp_pkt udp_pkt;

        fill_udp_packet (&udp_pkt);

        for (int probe = g_traceroute.info.probe;
             probe < g_traceroute.info.nprobes; ++probe)
        {
            if (sendto (g_traceroute.sock_info.send_fd, &udp_pkt,
                        sizeof (struct s_udp_pkt), 0,
                        (const struct sockaddr *)g_traceroute.sock_info.ip_addr, INET_ADDRSTRLEN)
                == -1)
            {
                perror ("sendto");
                release_resources ();
                exit (EXIT_FAILURE);
            }
        }

        TRACEROUTE_DEBUG("Traceroute has been successfully terminated.");
        release_resources ();
    }
}