#include "ft_traceroute.h"

double
compute_elapsed_ms (struct timespec start, struct timespec end)
{
    double elapsed_sec = (double)(end.tv_sec - start.tv_sec) * 1000.0;
    double elapsed_nsec;

    if (end.tv_nsec >= start.tv_nsec)
    {
        elapsed_nsec = (double)(end.tv_nsec - start.tv_nsec) / 1e6;
    }
    else
    {
        elapsed_sec -= 1000.0;
        elapsed_nsec = (double)(end.tv_nsec + 1000000000 - start.tv_nsec) / 1e6;
    }

    return elapsed_sec + elapsed_nsec;
}