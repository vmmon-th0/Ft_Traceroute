#include "ft_traceroute.h"

struct s_traceroute g_traceroute;

static char short_config[] = "hq:m:p:";

static struct option long_config[]
    = { { "help", no_argument, NULL, 'h' },
        { "queries", required_argument, NULL, 'q' },
        { "max-hops", required_argument, NULL, 'm' },
        { "port", required_argument, NULL, 'p' } };

static void
show_usage (void)
{
    printf ("\
Usage: traceroute [OPTION]... [ADDRESS]...\n\
config :\n\
  -h, --help        Display this help and exit\n\
  -q, --queries     Sets the number of probe packets per hop. The default is 3. (max 10)\n\
  -m, --max-hops    Specifies the maximum number of hops (max time-to-live value) traceroute will probe. The default is 30. (max 255)\n\
  -p, --port        For UDP tracing, specifies the destination port base traceroute will use (the destination port number will be incremented by each probe). The default is 32768 + 666. (max 65535)\n");
}

static void
show_usage_and_exit (int exit_code)
{
    show_usage ();
    troute_exit (exit_code);
}

static int
is_running_as_root ()
{
    return getuid () == 0;
}

static void
handle_sig (int sig)
{
    if (sig == SIGALRM)
    {
        TRACEROUTE_DEBUG ("SIGALRM received\n");
        g_traceroute.info.got_alarm = true;
    }
    else if (sig == SIGINT)
    {
        troute_exit (EXIT_SUCCESS);
    }
}

int
main (int argc, char *argv[])
{
    int opt;
    int long_index;
    struct sigaction sa;

    long_index = 0;

    if (!is_running_as_root ())
    {
        fprintf (stderr, "Program needs to be run as root\n");
        troute_exit (EXIT_FAILURE);
    }

    sa.sa_handler = handle_sig;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction (SIGALRM, &sa, NULL) == -1)
    {
        perror ("sigaction");
        return -1;
    }

    if (sigaction (SIGINT, &sa, NULL) == -1)
    {
        perror ("sigaction SIGINT");
        return -1;
    }

    traceroute_init_g_info ();

    while ((opt = getopt_long (argc, argv, short_config, long_config,
                               &long_index))
           != -1)
    {
        char *endptr;
        switch (opt)
        {
            case 'h':
            {
                show_usage_and_exit (EXIT_SUCCESS);
            }
            case 'q':
            {
                long nqueries = strtol(optarg, &endptr, 10);
                if (errno == ERANGE || nqueries < 0 || nqueries > 10 || *endptr != '\0')
                {
                    fprintf(stderr, "invalid nqueries value: %s\n", optarg);
                    show_usage_and_exit(EXIT_FAILURE);
                }
                g_traceroute.info.config.nqueries = nqueries;
                break;
            }
            case 'm':
            {
                long maxhops = strtol(optarg, &endptr, 10);
                if (errno == ERANGE || maxhops < 0 || maxhops > UINT8_MAX || *endptr != '\0')
                {
                    fprintf(stderr, "invalid maxhops value: %s\n", optarg);
                    show_usage_and_exit(EXIT_FAILURE);
                }
                g_traceroute.info.config.max_ttl = maxhops;
                break;
            }
            case 'p':
            {
                long port = strtol(optarg, &endptr, 10);
                if (errno == ERANGE || port < 0 || port > UINT16_MAX || *endptr != '\0')
                {
                    fprintf(stderr, "invalid port value: %s\n", optarg);
                    show_usage_and_exit(EXIT_FAILURE);
                }
                g_traceroute.info.config.port = port;
                break;
            }
            default:
            {
                fprintf (stderr, "Unknown option: -%c\n", optopt);
                show_usage_and_exit (EXIT_FAILURE);
            }
        }
    }

    if (optind != argc - 1)
    {
        show_usage_and_exit (EXIT_FAILURE);
    }

    argv += optind;
    traceroute_coord (*argv);
    return (troute_exit (EXIT_SUCCESS), EXIT_SUCCESS); // hmmmm
}