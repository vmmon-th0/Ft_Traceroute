#include "ft_traceroute.h"

struct s_traceroute g_traceroute;

static char short_options[] = "hIT";

static struct option long_options[] = { { "help", no_argument, NULL, 'h' },
                                        { "icmp", no_argument, NULL, 'I' },
                                        { "tcp", no_argument, NULL, 'T' },
                                        { NULL, 0, NULL, 0 } };

static void
show_usage (void)
{
    printf ("\
Usage: traceroute [OPTION]... [ADDRESS]...\n\
Options :\n\
  -h, --help        Display this help and exit\n\
  -I, --icmp        Use ICMP ECHO for probes\n\
  -T, --tcp         Use TCP SYN for probes\n");
}

static void
show_usage_and_exit (int exit_code)
{
    show_usage ();
    exit (exit_code);
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
        g_traceroute.info.ready_send = true;
    }
    else if (sig == SIGINT)
    {
        release_resources ();
        exit (EXIT_SUCCESS);
    }
}

int
main (int argc, char *argv[])
{
    int opt;
    int long_index;

    long_index = 0;

    if (!is_running_as_root ())
    {
        fprintf (stderr, "Program needs to be run as root\n");
        exit (EXIT_FAILURE);
    }

    signal (SIGINT, handle_sig);
    signal (SIGALRM, handle_sig);
    traceroute_init_g_info ();

    while ((opt = getopt_long (argc, argv, short_options, long_options,
                               &long_index))
           != -1)
    {
        switch (opt)
        {
            case 'h':
            {
                show_usage_and_exit (EXIT_SUCCESS);
            }
            case 'T':
            {
                g_traceroute.options.tcp = true;
                break;
            }
            case 'I':
            {
                g_traceroute.options.icmp = true;
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
    return g_traceroute.info.exit_code;
}