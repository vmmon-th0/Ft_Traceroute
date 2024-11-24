#include "ft_traceroute.h"

/**
 * @brief This function resolves a hostname that will be the traceroute destination
 * target, we are dealing with IPV4, The getaddrinfo() function allocates
 * and initializes a linked list of addrinfo structures. There are several
 * reasons why the linked list may have more than one addrinfo structure :
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
        exit(EXIT_FAILURE);
    }

    for (p = res; p != NULL; p = p->ai_next)
    {
        if (p->ai_family == AF_INET)
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            void *addr = &(ipv4->sin_addr);

            if (inet_ntop (p->ai_family, addr, g_traceroute.sock_info.ip_addr,
                           sizeof (g_traceroute.sock_info.ip_addr)) == NULL)
            {
                perror ("inet_ntop IPv4");
                continue;
            }
            memcpy (&g_traceroute.sock_info.ai, p, sizeof (struct addrinfo));
            // TRACEROUTE_DEBUG ("IPv4 addr for %s: %s\n", hostname, g_traceroute.sock_info.ip_addr);
            break;
        }
    }

    if (res->ai_canonname)
    {
        // TRACEROUTE_DEBUG ("Canonname for IPv4 addr %s: %s\n", g_traceroute.sock_info.ip_addr, res->ai_canonname);
        memcpy (g_traceroute.sock_info.ai_canonname, res->ai_canonname,
                sizeof (g_traceroute.sock_info.ai_canonname));
        g_traceroute.sock_info.ai.ai_canonname = g_traceroute.sock_info.ai_canonname;
    }

    freeaddrinfo (res);

    if (p == NULL)
    {
        fprintf (stderr, "No valid IPv4 address found for %s\n",
                 hostname);
        exit(EXIT_FAILURE);
    }

    g_traceroute.sock_info.hostname = hostname;
}

void
traceroute_coord (const char *hostname)
{
    resolve_hostname(hostname);
    traceroute_socket_init ();

    // Implement loop for tracerouting
}