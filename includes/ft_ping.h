#ifndef FT_PING_H
# define FT_PING_H

/* Standard includes */
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>
# include <signal.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <netdb.h>
# include <netinet/ip.h>
# include <netinet/ip_icmp.h>
# include <netinet/in.h>
# include <errno.h>
# include <stdbool.h>

/* Default values */
# define PACKET_SIZE 64              /* Default packet size */
# define DEFAULT_TTL 64              /* Default Time-To-Live */
# define DEFAULT_TIMEOUT 1           /* Default timeout in seconds */
# define DEFAULT_INTERVAL 1          /* Default interval between pings in seconds */
# define DEFAULT_PING_COUNT 0        /* Default ping count (0 = unlimited) */

/* ICMP definitions */
# define ICMP_ECHO_REQUEST 8         /* ICMP echo request */
# define ICMP_ECHO_REPLY 0           /* ICMP echo reply */

/* Error codes */
# define ERR_SOCKET 1
# define ERR_SETOPT 2
# define ERR_ADDR 3
# define ERR_PERMISSION 4
# define ERR_SENDTO 5

/* Global variables */
extern volatile int g_ping_count;    /* Number of pings sent */
extern volatile int g_ping_received; /* Number of pings received */
extern volatile bool g_running;      /* Program running flag */

/* Type definitions */
typedef struct s_ping_stats {
    double min_time;                /* Minimum round-trip time */
    double max_time;                /* Maximum round-trip time */
    double total_time;              /* Total round-trip time */
    int packets_sent;               /* Number of packets sent */
    int packets_received;           /* Number of packets received */
    char *hostname;                 /* Target hostname (for display) */
} t_ping_stats;

typedef struct s_options {
    bool verbose;                   /* Verbose output flag */
    bool help;                      /* Show help flag */
    char *target;                   /* Target host (IP or hostname) */
    struct sockaddr_in addr;        /* Target address */
    char *hostname;                 /* Hostname (for display) */
    char *ipstr;                    /* IP string (for display) */
} t_options;

/* Function prototypes */

/* main.c */
void print_usage(void);
void parse_args(int argc, char **argv, t_options *opts);
void signal_handler(int signo);

/* ping.c */
int start_ping(t_options *opts);
void finish_ping(t_ping_stats *stats);

/* socket.c */
int create_socket(void);
int setup_socket(int sockfd);

/* packet.c */
void init_packet(void *packet, int size, int seq);
uint16_t compute_checksum(void *addr, int count);
int send_packet(int sockfd, struct sockaddr_in *addr, void *packet, int size);
int receive_packet(int sockfd, struct sockaddr_in *addr, void *buffer, int size, struct timeval *tv);

/* dns.c */
int resolve_hostname(const char *hostname, struct sockaddr_in *addr, char **ipstr);
char *get_hostname_from_ip(struct sockaddr_in *addr);

/* display.c */
void print_ping_header(t_options *opts);
void print_ping_result(t_options *opts, int seq, int bytes, float rtt);
void print_ping_stats(t_ping_stats *stats);
void print_verbose(const char *format, ...);

#endif /* FT_PING_H */
