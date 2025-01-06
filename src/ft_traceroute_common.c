#include "ft_traceroute.h"

static void
release_resources ()
{
    close (g_traceroute.sock_info.send_fd);
    close (g_traceroute.sock_info.recv_fd);
}

void
troute_exit (int status)
{
    release_resources ();
    exit (status);
}