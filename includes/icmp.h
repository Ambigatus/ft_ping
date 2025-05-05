#ifndef ICMP_H
# define ICMP_H

# include <stdint.h>

/* ICMP packet types */
# define ICMP_ECHO_REPLY     0
# define ICMP_DEST_UNREACH   3
# define ICMP_SOURCE_QUENCH  4
# define ICMP_REDIRECT       5
# define ICMP_ECHO_REQUEST   8
# define ICMP_TIME_EXCEEDED  11
# define ICMP_PARAMETERPROB  12
# define ICMP_TIMESTAMP      13
# define ICMP_TIMESTAMPREPLY 14
# define ICMP_INFO_REQUEST   15
# define ICMP_INFO_REPLY     16
# define ICMP_ADDRESS        17
# define ICMP_ADDRESSREPLY   18

/* ICMP packet structure (RFC 792) */
struct icmp {
    uint8_t  icmp_type;        /* Type of message */
    uint8_t  icmp_code;        /* Code */
    uint16_t icmp_cksum;       /* Checksum */
    union {
        struct {
            uint16_t id;       /* Identifier */
            uint16_t sequence; /* Sequence number */
        } echo;                /* Echo datagram */
        uint32_t gateway;      /* Gateway address */
        struct {
            uint16_t unused;   /* Unused */
            uint16_t mtu;      /* MTU of next hop */
        } frag;                /* Path MTU discovery */
    } un;
    /* Followed by data */
};

/* ICMP packet structure for Linux */
struct icmphdr {
    uint8_t  type;             /* Message type */
    uint8_t  code;             /* Type sub-code */
    uint16_t checksum;         /* Checksum */
    union {
        struct {
            uint16_t id;       /* ID of process sending the Echo request */
            uint16_t sequence; /* Sequence number */
        } echo;                /* Echo datagram */
        uint32_t gateway;      /* Gateway address */
        struct {
            uint16_t __unused; /* Unused */
            uint16_t mtu;      /* MTU of next hop */
        } frag;                /* Path MTU discovery */
    } un;
};

#endif /* ICMP_H */