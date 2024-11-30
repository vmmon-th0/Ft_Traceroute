#include "ft_traceroute.h"

void
fill_udp_packet_v4 (struct s_traceroute_pkt *traceroute_pkt)
{
    static int sequence = 0;

    memset (ping_pkt, 0, sizeof (struct ping_packet_v4));
    ping_pkt->hdr.type = ICMP_ECHO;
    ping_pkt->hdr.code = 0;
    /* Using the mask ensures that the ID does not exceed 16 bits, which is a
     * convention for ICMP packets */
    ping_pkt->hdr.un.echo.id = htons (getpid () & 0xFFFF);
    ping_pkt->hdr.un.echo.sequence = htons (++sequence);
    /* Filling data payload with random data */
    memset (ping_pkt->data, 0xA5, sizeof (ping_pkt->data));
    /* Remember to set the checksum to 0 since it will be calculated on the
     * entire ICMP packet. */
    ping_pkt->hdr.checksum = 0;
    ping_pkt->hdr.checksum
        = compute_checksum_v4 (ping_pkt, sizeof (struct ping_packet_v4));
}