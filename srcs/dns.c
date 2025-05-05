#include "../includes/ft_ping.h"

/**
 * Resolve hostname to IP address
 *
 * @param hostname Hostname or IP address string
 * @param addr Address structure to fill (output)
 * @param ipstr IP address string (output, must be freed by caller)
 * @return 0 on success, -1 on error
 */
int resolve_hostname(const char *hostname, struct sockaddr_in *addr, char **ipstr)
{
    struct addrinfo hints, *res, *p;
    int status;
    char ip[INET_ADDRSTRLEN];

    /* Set up hints for getaddrinfo */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;     /* IPv4 only */
    hints.ai_socktype = SOCK_RAW;  /* Raw socket */

    /* Get address info */
    if ((status = getaddrinfo(hostname, NULL, &hints, &res)) != 0) {
        return -1;
    }

    /* Get the first IPv4 address */
    for (p = res; p != NULL; p = p->ai_next) {
        if (p->ai_family == AF_INET) {
            /* Copy address */
            memcpy(addr, p->ai_addr, sizeof(struct sockaddr_in));

            /* Convert IP to string */
            inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN);
            *ipstr = strdup(ip);

            freeaddrinfo(res);
            return 0;
        }
    }

    /* No suitable address found */
    freeaddrinfo(res);
    return -1;
}

/**
 * Get hostname from IP address (reverse DNS lookup)
 * Note: This function is not used in the packet return as per project requirements,
 *       but can be used for display purposes.
 *
 * @param addr Address structure
 * @return Hostname or NULL on error (must be freed by caller)
 */
char *get_hostname_from_ip(struct sockaddr_in *addr)
{
    char host[NI_MAXHOST];
    int ret;

    /* Perform reverse DNS lookup */
    ret = getnameinfo((struct sockaddr *)addr, sizeof(struct sockaddr_in),
                      host, NI_MAXHOST, NULL, 0, NI_NAMEREQD);

    if (ret != 0) {
        return NULL;
    }

    return strdup(host);
}
