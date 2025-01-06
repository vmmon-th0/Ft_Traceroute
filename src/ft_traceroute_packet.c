#include "ft_traceroute.h"

/**
 * @brief Compute a checksum for data integrity.
 *
 * This function computes a checksum for the given data to ensure its integrity.
 * It performs the following operations:
 *
 * 1. Adjacent octets to be checksummed are paired to form 16-bit integers,
 *    and the 1's complement sum of these 16-bit integers is calculated.
 * 2. To generate a checksum, the checksum field itself is cleared, the
 *    16-bit 1's complement sum is computed over the octets concerned, and
 *    the 1's complement of this sum is placed in the checksum field.
 * 3. To check a checksum, the 1's complement sum is computed over the same
 *    set of octets, including the checksum field. If the result is all 1 bits
 *    (i.e., -0 in 1's complement arithmetic), the check succeeds.
 *
 * @param s_troute_pkt Pointer to the `s_troute_pkt` structure whose data is to
 * be checksummed.
 * @param len Size of the `s_troute_pkt` structure.
 *
 * @return The computed checksum as a 16-bit integer.
 */

static uint16_t
compute_checksum_v4 (const uint16_t *data, size_t len)
{
    uint32_t sum = 0;

    /* Add 16-bit words */
    for (size_t i = 0; i < len / 2; ++i)
    {
        sum += data[i];
    }

    /* Carry bit (if the length is odd, add the last byte) */
    if (len % 2)
    {
        sum += ((const uint8_t *)data)[len - 1] << 8;
    }

    /* Fold 32-bit sum to 16 bits */
    while (sum >> 16)
    {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}

_Bool
verify_checksum (struct icmphdr *icmphdr, size_t pkt_size)
{
    uint16_t received_checksum = icmphdr->checksum;
    icmphdr->checksum = 0;
    uint16_t computed_checksum
        = compute_checksum_v4 ((const void *)icmphdr, pkt_size);
    return (computed_checksum == received_checksum);
}

void
fill_troute_packet (struct s_troute_pkt *troute_pkt, int ttl)
{
    memset (troute_pkt, 0, sizeof (struct s_troute_pkt));

    // ip header
    troute_pkt->iphdr.frag_off = htons (IP_DF);
    troute_pkt->iphdr.ttl = ttl;
    troute_pkt->iphdr.protocol = IPPROTO_UDP;
    troute_pkt->iphdr.saddr = INADDR_ANY;
    troute_pkt->iphdr.daddr = inet_addr (g_traceroute.sock_info.ip_addr);
    troute_pkt->iphdr.id = htons (getpid () & 0xFFFF);
    troute_pkt->iphdr.tot_len = htons (sizeof (struct s_troute_pkt));
    troute_pkt->iphdr.tos = 0;
    troute_pkt->iphdr.version = 4;
    troute_pkt->iphdr.ihl = 5;
    troute_pkt->iphdr.check = 0;
    troute_pkt->iphdr.check = compute_checksum_v4 (
        (const void *)&troute_pkt->iphdr, sizeof (struct iphdr));

    // udp header
    troute_pkt->udphdr.len = htons (sizeof (struct udphdr));
    troute_pkt->udphdr.source = htons (g_traceroute.info.srcp);
    troute_pkt->udphdr.dest = htons (g_traceroute.info.dstp + ttl);
    troute_pkt->udphdr.check = 0;
}