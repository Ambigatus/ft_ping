#include "ft_ping.h"

/**
 * Initialize an ICMP echo request packet
 *
 * @param packet Buffer to initialize
 * @param size Size of the buffer
 * @param seq Sequence number
 */
void init_packet(void *packet, int size, int seq)
{
    struct icmphdr *icmp;
    char *data;
//    int i;
    size_t data_size;

    /* Clear the packet buffer */
    memset(packet, 0, size);

    /* Set up ICMP header */
    icmp = (struct icmphdr *)packet;
    icmp->type = ICMP_ECHO_REQUEST;
    icmp->code = 0;
    icmp->un.echo.id = htons(getpid() & 0xFFFF); /* Use process ID as identifier */
    icmp->un.echo.sequence = htons(seq);

    /* Calculate data size and fill the data section with a pattern */
    data = (char *)packet + sizeof(struct icmphdr);
    data_size = size - sizeof(struct icmphdr);
    for (size_t i = 0; i < data_size; i++) {
        data[i] = 'a' + (i % 26);
    }

    /* Calculate and set the checksum */
    icmp->checksum = 0;
    icmp->checksum = compute_checksum(packet, size);
}

/**
 * Compute the checksum of an ICMP packet
 *
 * @param addr Buffer to compute checksum for
 * @param count Number of bytes
 * @return Checksum value
 */
uint16_t compute_checksum(void *addr, int count)
{
    uint32_t sum = 0;
    uint16_t *ptr = addr;

    /* Sum up 16-bit words */
    while (count > 1) {
        sum += *ptr++;
        count -= 2;
    }

    /* Add left-over byte, if any */
    if (count > 0) {
        sum += *(uint8_t *)ptr;
    }

    /* Fold 32-bit sum to 16 bits */
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}

/**
 * Send an ICMP packet
 *
 * @param sockfd Socket file descriptor
 * @param addr Target address
 * @param packet Packet to send
 * @param size Size of the packet
 * @return Number of bytes sent or -1 on error
 */
int send_packet(int sockfd, struct sockaddr_in *addr, void *packet, int size)
{
    int ret;

    ret = sendto(sockfd, packet, size, 0,
                 (struct sockaddr *)addr, sizeof(struct sockaddr_in));
    if (ret < 0) {
        perror("sendto");
        return -1;
    }

    return ret;
}

/**
 * Receive and validate an ICMP response packet
 *
 * @param sockfd Socket file descriptor
 * @param addr Source address (output)
 * @param buffer Buffer to store received data
 * @param size Size of the buffer
 * @param tv Timeout value
 * @return Number of bytes received or -1 on error
 */
int receive_packet(int sockfd, struct sockaddr_in *addr, void *buffer, int size, struct timeval *tv)
{
    int ret, addr_len, hlen;
    fd_set readfds;
    struct ip *ip;
    struct icmphdr *icmp;
    uint16_t received_id;

    /* Set up select for timeout */
    FD_ZERO(&readfds);
    FD_SET(sockfd, &readfds);

    /* Wait for data with timeout */
    ret = select(sockfd + 1, &readfds, NULL, NULL, tv);
    if (ret <= 0) {
        return -1; /* Timeout or error */
    }

    /* Receive packet */
    addr_len = sizeof(struct sockaddr_in);
    ret = recvfrom(sockfd, buffer, size, 0,
                   (struct sockaddr *)addr, (socklen_t *)&addr_len);
    if (ret < 0) {
        if (errno != EAGAIN && errno != EINTR) {
            perror("recvfrom");
        }
        return -1;
    }

    /* Extract IP and ICMP headers */
    ip = (struct ip *)buffer;
    hlen = ip->ip_hl << 2; /* IP header length in bytes */

    /* Check if we have a complete ICMP header */
    if ((size_t)ret < hlen + sizeof(struct icmphdr)) {
        return -1;
    }

    /* Get ICMP header from the received packet */
    icmp = (struct icmphdr *)((char *)buffer + hlen);

    /* Validate that it's an ICMP echo reply */
    if (icmp->type != ICMP_ECHO_REPLY) {
        /* If verbose is enabled, we could print info about other ICMP types here */
        return -1;
    }

    /* Extract ID (in network byte order) */
    received_id = ntohs(icmp->un.echo.id);

    /* Validate that it's a response to our request (check PID) */
    if (received_id != (getpid() & 0xFFFF)) {
        return -1;
    }

    /* Verify checksum */
    if (compute_checksum(icmp, ret - hlen) != 0) {
        /* If checksum is incorrect, packet might be corrupted */
        return -1;
    }

    return ret;
}
