#include "../includes/ft_ping.h"

/**
 * Create a raw socket for ICMP
 *
 * @return Socket file descriptor or -1 on error
 */
int create_socket(void)
{
    int sockfd;

    /* Create raw socket for ICMP protocol */
    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    return sockfd;
}

/**
 * Set up socket options
 *
 * @param sockfd Socket file descriptor
 * @return 0 on success, -1 on error
 */
int setup_socket(int sockfd)
{
    int ttl = DEFAULT_TTL;
    struct timeval timeout;

    /* Set TTL (Time-To-Live) */
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)) < 0) {
        perror("setsockopt IP_TTL");
        return -1;
    }

    /* Set receive timeout */
    timeout.tv_sec = DEFAULT_TIMEOUT;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("setsockopt SO_RCVTIMEO");
        return -1;
    }

    return 0;
}
