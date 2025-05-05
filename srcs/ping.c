#include "../includes/ft_ping.h"

/**
 * Start pinging the target
 *
 * @param opts Options structure
 * @return Exit code
 */
int start_ping(t_options *opts)
{
    int sockfd;
    int seq = 0;
    struct timeval send_time, recv_time, timeout, wait_time;
    double rtt;
    t_ping_stats stats;
    char packet[PACKET_SIZE];
    char recv_buffer[PACKET_SIZE + sizeof(struct ip) + 8]; /* Adding some padding */
    struct sockaddr_in recv_addr;
    int ret, bytes_recv;

    /* Initialize statistics */
    memset(&stats, 0, sizeof(t_ping_stats));
    stats.min_time = -1; /* Will be updated on first received packet */
    stats.hostname = strdup(opts->hostname); /* Copy hostname for statistics display */

    /* Create raw socket */
    sockfd = create_socket();
    if (sockfd < 0) {
        fprintf(stderr, "ft_ping: cannot create socket\n");
        return ERR_SOCKET;
    }

    /* Set socket options */
    if (setup_socket(sockfd) < 0) {
        fprintf(stderr, "ft_ping: cannot set socket options\n");
        close(sockfd);
        return ERR_SETOPT;
    }

    /* Print ping header */
    print_ping_header(opts);

    /* Main ping loop */
    while (g_running) {
        /* Initialize packet */
        init_packet(packet, PACKET_SIZE, seq);

        /* Record send time */
        gettimeofday(&send_time, NULL);

        /* Send packet */
        ret = send_packet(sockfd, &opts->addr, packet, PACKET_SIZE);
        if (ret < 0) {
            if (opts->verbose) {
                print_verbose("Error sending packet: %s", strerror(errno));
            }
        } else {
            g_ping_count++;
            stats.packets_sent++;

            /* Wait for response with timeout */
            timeout.tv_sec = DEFAULT_TIMEOUT;
            timeout.tv_usec = 0;

            bytes_recv = receive_packet(sockfd, &recv_addr, recv_buffer,
                                        sizeof(recv_buffer), &timeout);

            if (bytes_recv > 0) {
                /* Record receive time */
                gettimeofday(&recv_time, NULL);

                /* Calculate round-trip time in milliseconds */
                rtt = (recv_time.tv_sec - send_time.tv_sec) * 1000.0 +
                      (recv_time.tv_usec - send_time.tv_usec) / 1000.0;

                /* Update statistics */
                g_ping_received++;
                stats.packets_received++;
                stats.total_time += rtt;

                if (stats.min_time < 0 || rtt < stats.min_time) {
                    stats.min_time = rtt;
                }
                if (rtt > stats.max_time) {
                    stats.max_time = rtt;
                }

                /* Print ping result */
                print_ping_result(opts, seq, bytes_recv - sizeof(struct ip), rtt);
            } else {
                /* Handle timeout or error */
                if (opts->verbose) {
                    print_verbose("No response received within timeout");
                }
            }
        }

        /* Increment sequence number */
        seq++;

        /* Wait before sending next packet (while checking for interrupt) */
        gettimeofday(&wait_time, NULL);
        wait_time.tv_sec += DEFAULT_INTERVAL;

        while (g_running) {
            struct timeval current_time;
            gettimeofday(&current_time, NULL);

            /* Calculate time difference */
            if (current_time.tv_sec > wait_time.tv_sec ||
                (current_time.tv_sec == wait_time.tv_sec &&
                 current_time.tv_usec >= wait_time.tv_usec)) {
                break; /* Time to send next packet */
            }

            /* Sleep for a short time */
            usleep(10000); /* 10ms */
        }
    }

    /* Print final statistics */
    finish_ping(&stats);

    /* Free allocated memory */
    if (stats.hostname) {
        free(stats.hostname);
    }

    /* Close socket */
    close(sockfd);

    return 0;
}

/**
 * Finish pinging and print statistics
 *
 * @param stats Ping statistics
 */
void finish_ping(t_ping_stats *stats)
{
    double packet_loss;

    /* Calculate packet loss percentage */
    if (stats->packets_sent > 0) {
        packet_loss = 100.0 * (stats->packets_sent - stats->packets_received) / stats->packets_sent;
    } else {
        packet_loss = 0.0;
    }

    /* Print statistics in a format matching inetutils-2.0 */
    printf("\n--- %s ping statistics ---\n",
           stats->hostname ? stats->hostname : "ping");

    printf("%d packets transmitted, %d received, %.1f%% packet loss, time %dms\n",
           stats->packets_sent,
           stats->packets_received,
           packet_loss,
           (int)stats->total_time);

    /* Print round-trip statistics if packets were received */
    if (stats->packets_received > 0) {
        printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/0.000 ms\n",
               stats->min_time,
               stats->total_time / stats->packets_received,
               stats->max_time);
    }
}
