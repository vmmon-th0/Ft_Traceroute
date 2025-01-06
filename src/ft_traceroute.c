#include "ft_traceroute.h"

struct s_traceroute g_traceroute;

static char short_options[] = "h";

static struct option long_options[]
    = { { "help", no_argument, NULL, 'h' }, { NULL, 0, NULL, 0 } };

static void
show_usage (void)
{
    printf ("\
Usage: traceroute [OPTION]... [ADDRESS]...\n\
Options :\n\
  -h, --help        Display this help and exit\n");
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

    long_index = 0;

    if (!is_running_as_root ())
    {
        fprintf (stderr, "Program needs to be run as root\n");
        troute_exit (EXIT_FAILURE);
    }

    struct sigaction sa;
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