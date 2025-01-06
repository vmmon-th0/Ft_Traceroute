#include "ft_traceroute.h"

char recv_pkt[BUFFER_SIZE];

static int
recv_packet (struct sockaddr_in *r_addr)
{
    int recv_bytes;

    struct iphdr *ip_hdr;
    struct icmphdr *icmp_hdr;

    socklen_t addr_len;
    addr_len = sizeof (struct sockaddr_in);

    g_traceroute.info.got_alarm = false;
    alarm (3);

    for (;;)
    {
        if (g_traceroute.info.got_alarm == true)
        {
            TRACEROUTE_DEBUG ("time out reached, got alarm\n");
            return -3;
        }

        recv_bytes = recvfrom (g_traceroute.sock_info.recv_fd, recv_pkt,
                               sizeof (recv_pkt), 0, (struct sockaddr *)r_addr,
                               &addr_len);
        TRACEROUTE_DEBUG ("recv_bytes: %d\n", recv_bytes);

        if (recv_bytes < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror ("recvfrom");
            troute_exit (EXIT_FAILURE);
        }

        clock_gettime (CLOCK_MONOTONIC, &g_traceroute.info.rtt_metrics.end);
        ip_hdr = (struct iphdr *)recv_pkt;
        icmp_hdr = (struct icmphdr *)(recv_pkt + ip_hdr->ihl * 4);

        size_t pkt_size = recv_bytes - sizeof (struct iphdr);

        if (verify_checksum (icmp_hdr, pkt_size) == false)
        {
            fprintf (stderr, "Received corrupted ICMPv4 packet\n");
            troute_exit (EXIT_FAILURE);
        }

        if (icmp_hdr->type == ICMP_TIME_EXCEEDED
            || icmp_hdr->type == ICMP_DEST_UNREACH)
        {
            return icmp_hdr->type;
        }
        else
        {
            TRACEROUTE_DEBUG ("Another ICMP type received\n");
            break;
        }
    }
    alarm (0);
    return 0;
}

void
traceroute_coord (const char *hostname)
{
    resolve_hostname (hostname);

    sock_send_init ();
    sock_recv_init ();

    printf ("traceroute to %s (%s): %d hops max, %lu data bytes\n",
            strlen (g_traceroute.sock_info.ai_canonname)
                ? g_traceroute.sock_info.ai_canonname
                : g_traceroute.sock_info.hostname,
            g_traceroute.sock_info.ip_addr, g_traceroute.info.config.max_ttl,
            sizeof (struct s_troute_pkt));

    int dest_reach = 0;

    for (int ttl = 1; ttl <= g_traceroute.info.config.max_ttl && dest_reach == 0;
         ++ttl)
    {
        printf ("%d ", ttl);
        for (int probe = 0; probe < g_traceroute.info.config.nqueries; ++probe)
        {
            struct s_troute_pkt troute_pkt;
            fill_troute_packet (&troute_pkt, ttl);

            if (sendto (g_traceroute.sock_info.send_fd, &troute_pkt,
                        sizeof (struct s_troute_pkt), 0,
                        (const struct sockaddr *)&g_traceroute.sock_info.addr_4,
                        sizeof (g_traceroute.sock_info.addr_4))
                == -1)
            {
                perror ("sendto");
                troute_exit (EXIT_FAILURE);
            }

            clock_gettime (CLOCK_MONOTONIC,
                           &g_traceroute.info.rtt_metrics.start);

            TRACEROUTE_DEBUG (
                "probes no. %d has been successfully send with TTL: %d.\n",
                probe, ttl);

            int ret;
            struct sockaddr_in r_addr;

            if ((ret = recv_packet (&r_addr)) == -3)
            {
                printf ("* ");
                TRACEROUTE_DEBUG ("timeout reached");
            }
            else
            {
                if (probe == 0
                    || g_traceroute.info.last_sa.sin_addr.s_addr
                           != r_addr.sin_addr.s_addr)
                {
                    struct in_addr src_addr;
                    char src_ip[INET_ADDRSTRLEN];
                    char fqdn[NI_MAXHOST];

                    src_addr.s_addr = r_addr.sin_addr.s_addr;
                    inet_ntop (AF_INET, &src_addr, src_ip, INET_ADDRSTRLEN);
                    fqdn_resolver (src_ip, fqdn, sizeof (fqdn));

                    printf ("%s (%s) ", fqdn, src_ip);
                }

                printf ("%.3f ms ",
                        compute_elapsed_ms (g_traceroute.info.rtt_metrics.start,
                                            g_traceroute.info.rtt_metrics.end));

                memcpy (&g_traceroute.info.last_sa, &r_addr,
                        sizeof (struct sockaddr_in));
                TRACEROUTE_DEBUG ("s_addr:\n");
                TRACEROUTE_DEBUG ("lastsa: %d, r_addr: %d\n",
                                  g_traceroute.info.last_sa.sin_addr.s_addr,
                                  r_addr.sin_addr.s_addr);

                if (ret == ICMP_DEST_UNREACH)
                {
                    TRACEROUTE_DEBUG ("destination reached");
                    dest_reach = ICMP_DEST_UNREACH;
                }
            }
        }
        printf ("\n");
    }

    TRACEROUTE_DEBUG ("Traceroute has been successfully terminated.");
}