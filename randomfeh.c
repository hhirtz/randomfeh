#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define FEH_BIN "/usr/bin/swaybg"

static void
die(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    exit(1);
}

static struct timespec
parse_interval(const char *s)
{
    char *end_interval;
    long int interval = strtol(s, &end_interval, 10);
    struct timespec res;

    if (s == end_interval)
        die("The first argument must be an integer.\n");
    if (interval <= 0)
        die("The first argument must be > 0.\n");

    res.tv_sec = (time_t) interval;
    res.tv_nsec = 0;
    return res;
}

int
main(int argc, char **argv)
{
    if (argc < 3)
        die("Usage: %s TIME PATHS...\n", argv[0]);

    struct timespec interval = parse_interval(argv[1]);
    struct timespec wait_for_swaybg = { .tv_sec = 0, .tv_nsec = 500000000 };
    char *feh_argv[4];
    feh_argv[0] = FEH_BIN;
    feh_argv[1] = "-i";
    feh_argv[3] = 0;

    srand(time(0));
    argc -= 2;
    for (pid_t previous = 0;;) {
        int i = (rand() % argc) + 2;
        feh_argv[2] = argv[i];

        pid_t p = fork();
        if (p < 0)
            die("fork: %s\n", strerror(errno));
        if (p == 0) {
            execv(FEH_BIN, feh_argv);
            exit(0);
        }
        if (nanosleep(&wait_for_swaybg, 0) < 0)
            die("nanosleep: %s\n", strerror(errno));
        if (previous) {
            kill(previous, SIGKILL);
            waitpid(previous, 0, 0);
        }
        if (nanosleep(&interval, 0) < 0)
            die("nanosleep: %s\n", strerror(errno));
        previous = p;
    }

    return 0;
}
