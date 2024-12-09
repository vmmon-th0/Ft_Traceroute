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
 * @param s_udp_pkt Pointer to the `s_udp_pkt` structure whose data is to be
 * checksummed.
 * @param len Size of the `s_udp_pkt` structure.
 *
 * @return The computed checksum as a 16-bit integer.
 */

static uint16_t
compute_checksum_v4 (struct s_udp_pkt *udp_pkt, size_t len)
{
    const uint16_t *data = (const void *)udp_pkt;
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

void
fill_udp_packet (struct s_udp_pkt *udp_pkt)
{
    static int sequence = 0;

    memset (udp_pkt, 0, sizeof (struct s_udp_pkt));
    /* Filling data payload with random data */
    memset (udp_pkt->data, 0xA5, sizeof (udp_pkt->data));
    /* Remember to set the checksum to 0 since it will be calculated on the
     * entire ICMP packet. */
    udp_pkt->udphdr.len = htons (sizeof (struct udphdr)); // Put data length
    udp_pkt->udphdr.source = htons (g_traceroute.info.srcp);
    udp_pkt->udphdr.dest = htons (g_traceroute.info.dstp++);
    udp_pkt->udphdr.check = 0;
    udp_pkt->udphdr.check
        = compute_checksum_v4 (udp_pkt, sizeof (struct s_udp_pkt));
}