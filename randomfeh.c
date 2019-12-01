#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define FEH_BIN "/usr/bin/feh"

static inline void
die(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
    exit(1);
}

static inline struct timespec
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

static inline void
spawn(const char *bin, char **argv)
{
    pid_t p = fork();
    if (p < 0)
        die("fork: %s\n", strerror(errno));
    if (p == 0) {
        execv(bin, argv);
        exit(0);
    }
    wait(0);
}

int
main(int argc, char **argv)
{
    if (argc < 3)
        die("Usage: %s TIME PATHS...\n", argv[0]);

    struct timespec interval = parse_interval(argv[1]);
    char *feh_argv[argc + 2];
    feh_argv[0] = FEH_BIN;
    feh_argv[1] = "-z";
    feh_argv[2] = "--bg-fill";
    for (int i = 2; i < argc; i++)
        feh_argv[i + 1] = argv[i];
    feh_argv[argc + 1] = 0;

    for (;;) {
        spawn(FEH_BIN, feh_argv);
        if (nanosleep(&interval, 0) < 0)
            die("nanosleep: %s\n", strerror(errno));
    }

    return 0;
}
