#include "ft_traceroute.h"

void
release_resources ()
{
    close (g_traceroute.sock_info.send_fd);
    close (g_traceroute.sock_info.recv_fd);
}