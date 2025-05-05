#include "../includes/ft_ping.h"
#include <stdarg.h>

/**
 * Print ping header information
 *
 * @param opts Options structure
 */
void print_ping_header(t_options *opts)
{
    printf("PING %s (%s) %zu(%d) bytes of data.\n",
           opts->hostname,
           opts->ipstr,
           PACKET_SIZE - sizeof(struct icmphdr),
           PACKET_SIZE);

    /* Flush stdout to ensure the header is displayed immediately */
    fflush(stdout);
}

/**
 * Print ping result for a single packet
 *
 * @param opts Options structure
 * @param seq Sequence number
 * @param bytes Bytes received
 * @param rtt Round-trip time in milliseconds
 */
void print_ping_result(t_options *opts, int seq, int bytes, float rtt)
{
    /* Display the basic ping result format - formatted exactly like inetutils-2.0 */
    printf("%d bytes from %s (%s): icmp_seq=%d ttl=%d time=%.3f ms\n",
           bytes,
           opts->hostname,
           opts->ipstr,
           seq,
           DEFAULT_TTL,
           rtt);

    /* Flush stdout to ensure each result is displayed immediately */
    fflush(stdout);
}

/**
 * Print ping statistics
 *
 * @param stats Ping statistics structure
 */
void print_ping_stats(t_ping_stats *stats)
{
    double packet_loss;

    /* Calculate packet loss percentage */
    if (stats->packets_sent > 0) {
        packet_loss = 100.0 * (stats->packets_sent - stats->packets_received) / stats->packets_sent;
    } else {
        packet_loss = 0.0;
    }

    /* Print statistics in a format matching inetutils-2.0 */
    printf("\n--- %s ping statistics ---\n", stats->hostname);
    printf("%d packets transmitted, %d received, %.1f%% packet loss, time %dms\n",
           stats->packets_sent,
           stats->packets_received,
           packet_loss,
           (int)stats->total_time);

    if (stats->packets_received > 0) {
        printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/0.000 ms\n",
               stats->min_time,
               stats->total_time / stats->packets_received,
               stats->max_time);
    }

    /* Flush stdout to ensure statistics are displayed immediately */
    fflush(stdout);
}

/**
 * Print verbose information (when -v option is used)
 *
 * @param format Format string (printf style)
 * @param ... Variable arguments
 */
void print_verbose(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    fprintf(stderr, "ft_ping: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);

    /* Flush stderr to ensure immediate display */
    fflush(stderr);
}
