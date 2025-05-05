#include "../includes/ft_ping.h"

/* Global variables */
volatile int g_ping_count = 0;
volatile int g_ping_received = 0;
volatile bool g_running = true;

/**
 * Print usage instructions
 */
void print_usage(void)
{
    printf("Usage: ft_ping [options] <destination>\n");
    printf("\nOptions:\n");
    printf("  -v                 verbose output\n");
    printf("  -?                 print help and exit\n");
    printf("\nArguments:\n");
    printf("  destination        dns name or ip address\n");
    exit(0);
}

/**
 * Parse command line arguments
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @param opts Options structure to fill
 */
void parse_args(int argc, char **argv, t_options *opts)
{
    int opt;
//    int option_index = 0;

    /* Set default values */
    memset(opts, 0, sizeof(t_options));

    /* Check for explicit help option before regular parsing */
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-?") == 0) {
            print_usage();
            exit(0);
        }
    }

    /* Parse options */
    while ((opt = getopt(argc, argv, "v")) != -1) {
        switch (opt) {
            case 'v':
                opts->verbose = true;
                break;
            default:
                fprintf(stderr, "Try 'ft_ping -?' for more information.\n");
                exit(1);
        }
    }

    /* Check if target is provided */
    if (optind >= argc) {
        fprintf(stderr, "ft_ping: missing host operand\n");
        fprintf(stderr, "Try 'ft_ping -?' for more information.\n");
        exit(1);
    }

    /* Get target hostname or IP */
    opts->target = argv[optind];
}

/**
 * Signal handler for graceful termination
 *
 * @param signo Signal number
 */
void signal_handler(int signo)
{
    if (signo == SIGINT) {
        /* SIGINT (Ctrl+C) - Stop pinging and print statistics */
        g_running = false;
    } else if (signo == SIGALRM) {
        /* SIGALRM - Used for timeout */
        /* Just return to let the program continue */
        return;
    } else if (signo == SIGTERM) {
        /* SIGTERM - Terminate immediately */
        g_running = false;
    }
}

/**
 * Main function
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit code
 */
int main(int argc, char **argv)
{
    t_options opts;
    int ret;

    /* Check if running as root or with capabilities */
    if (getuid() != 0) {
        fprintf(stderr, "ft_ping: Operation not permitted\n");
        return ERR_PERMISSION;
    }

    /* Initialize options structure */
    memset(&opts, 0, sizeof(t_options));

    /* Parse command line arguments */
    parse_args(argc, argv, &opts);

    /* Set up signal handler for graceful termination */
    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        fprintf(stderr, "ft_ping: Could not set signal handler\n");
        return 1;
    }

    if (signal(SIGALRM, signal_handler) == SIG_ERR) {
        fprintf(stderr, "ft_ping: Could not set signal handler\n");
        return 1;
    }

    /* Resolve hostname to IP address */
    if ((ret = resolve_hostname(opts.target, &opts.addr, &opts.ipstr)) != 0) {
        fprintf(stderr, "ft_ping: unknown host %s\n", opts.target);
        return ERR_ADDR;
    }

    /* Store original hostname for display */
    opts.hostname = strdup(opts.target);
    if (!opts.hostname) {
        fprintf(stderr, "ft_ping: memory allocation failed\n");
        free(opts.ipstr);
        return 1;
    }

    /* Print verbose information if requested */
    if (opts.verbose) {
        print_verbose("PING %s (%s) %d(%d) bytes of data.",
                      opts.hostname, opts.ipstr,
                      PACKET_SIZE - sizeof(struct icmphdr), PACKET_SIZE);
    }

    /* Start pinging */
    ret = start_ping(&opts);

    /* Clean up */
    free(opts.hostname);
    free(opts.ipstr);

    return ret;
}
