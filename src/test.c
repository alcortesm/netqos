#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define NETQOS_VERSION      "0.2"
#define NETQOS_PATH         "/sys/kernel/netqos/"
#define FIGURES_BASENAME    "figures/"
#define IFACES_BASENAME     "ifaces/"
#define VERSION_BASENAME    "version"
#define FIG_BW_BASENAME     "bw"
#define FIG_DELAY_BASENAME  "delay"
#define FIG_JITTER_BASENAME "jitter"
#define FIG_PRICE_BASENAME  "price"

char * argv0;
int debug = 0;

/* print and error message and exits */
void
fatal(char *fmt, ...)
{
    va_list val;
    char *prev_errstr;
    prev_errstr = strdup(strerror(errno));
    if (prev_errstr == NULL) {
        fprintf(stderr, "%s: virtual memory exhausted\n", argv0);
        fflush(NULL);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "%s -> ERROR: ", argv0);
    fprintf(stderr, "%s: ", prev_errstr);

    va_start(val, fmt);
    vfprintf(stderr, fmt, val);
    va_end(val);

    putc('\n', stderr);
    fflush(NULL);
    exit(EXIT_FAILURE);
}

/* this function report an error when no syscall has failed and exits */
void
error(char *fmt, ...)
{
    va_list val;

    fprintf(stderr, "%s -> ERROR: ", argv0);

    va_start(val, fmt);
    vfprintf(stderr, fmt, val);
    va_end(val);

    putc('\n', stderr);
    fflush(NULL);
    exit(EXIT_FAILURE);
}

void *
xmalloc (size_t size)
{
    register void * value;
    value = malloc (size);
    if (value == 0)
        fatal("malloc: virtual memory exhausted");
    return(value);
}

void *
xcalloc (size_t size)
{
    register void * value;
    value = calloc (size, 1);
    if (value == 0)
        fatal("calloc: virtual memory exhausted");
    return(value);
}

void *
xstrdup (const char *s)
{
    char * dup;
    dup = strdup(s);
    if (!dup)
        fatal("strdup: virtual memory exhausted");
    return(dup);
}

void
dprint(char* fmt, ...) {
    if (!debug)
        return;

    va_list val;

    fprintf(stderr, "%s -> DEBUG: ", argv0);

    va_start(val, fmt);
    vfprintf(stderr, fmt, val);
    va_end(val);
}

void
dprintn(char* fmt, ...) {
    if (!debug)
        return;

    va_list val;

    fprintf(stderr, "%s -> DEBUG: ", argv0);

    va_start(val, fmt);
    vfprintf(stderr, fmt, val);
    va_end(val);
    fprintf(stderr, "\n");
}

void
usage()
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "\t%s [-d]\n", argv0);
    fprintf(stderr, "\nUse -d for debugging\n");
    exit(EXIT_FAILURE);
}

int
main(int argc, char ** argv)
{
    (argv0 = strrchr(argv[0], '/')) ? argv0++ : (argv0 = argv[0]);

    if (argc > 2)
        usage();
    if (argc == 2) {
        if (!strcmp(argv[1], "-d"))
            debug = 1;
        else
            usage();
    }
        

    /* build paths */
    char * netqos_path;
    char * figures_path;
    char * ifaces_path;
    char * version_path;
    char * fig_bw_path;
    char * fig_delay_path;
    char * fig_jitter_path;
    char * fig_price_path;
    {
        size_t bufsz;
        bufsz = (PATH_MAX+1)*sizeof(char);

        char * buf;
        buf = xcalloc(bufsz);

        netqos_path = xstrdup(NETQOS_PATH);
        if (strlen(netqos_path) > PATH_MAX)
            error("netqos_path too long: %s", netqos_path);

        strcat(buf, NETQOS_PATH);
        strcat(buf, FIGURES_BASENAME);
        figures_path = xstrdup(buf);
        if (strlen(figures_path) > PATH_MAX)
            error("figures_path too long: %s", figures_path);
        memset(buf, '\0', bufsz);

        strcat(buf, NETQOS_PATH);
        strcat(buf, IFACES_BASENAME);
        ifaces_path = xstrdup(buf);
        if (strlen(ifaces_path) > PATH_MAX)
            error("ifaces_path too long: %s", ifaces_path);
        memset(buf, '\0', bufsz);

        strcat(buf, NETQOS_PATH);
        strcat(buf, VERSION_BASENAME);
        version_path = xstrdup(buf);
        if (strlen(version_path) > PATH_MAX)
            error("version_path too long: %s", version_path);
        memset(buf, '\0', bufsz);

        strcat(buf, NETQOS_PATH);
        strcat(buf, FIGURES_BASENAME);
        strcat(buf, FIG_BW_BASENAME);
        fig_bw_path = xstrdup(buf);
        if (strlen(fig_bw_path) > PATH_MAX)
            error("fig_bw_path too long: %s", fig_bw_path);
        memset(buf, '\0', bufsz);

        strcat(buf, NETQOS_PATH);
        strcat(buf, FIGURES_BASENAME);
        strcat(buf, FIG_DELAY_BASENAME);
        fig_delay_path = xstrdup(buf);
        if (strlen(fig_delay_path) > PATH_MAX)
            error("fig_delay_path too long: %s", fig_delay_path);
        memset(buf, '\0', bufsz);

        strcat(buf, NETQOS_PATH);
        strcat(buf, FIGURES_BASENAME);
        strcat(buf, FIG_JITTER_BASENAME);
        fig_jitter_path = xstrdup(buf);
        if (strlen(fig_jitter_path) > PATH_MAX)
            error("fig_jitter_path too long: %s", fig_jitter_path);
        memset(buf, '\0', bufsz);

        strcat(buf, NETQOS_PATH);
        strcat(buf, FIGURES_BASENAME);
        strcat(buf, FIG_PRICE_BASENAME);
        fig_price_path = xstrdup(buf);
        if (strlen(fig_price_path) > PATH_MAX)
            error("fig_price_path too long: %s", fig_price_path);
        memset(buf, '\0', bufsz);

        free(buf);
    }

    /* check if module is loaded */
    {
        int r;
        r = system("lsmod | egrep '^netqos .*$' >/dev/null 2>&1");
        if (r)
            error("test 000 failed");
    }

    /* check if netqos main dir is present */
    {
        DIR * dirp;
        dprintn("trying to open %s", netqos_path);
        dirp = opendir(netqos_path);
        if (!dirp)
            fatal("test 001 failed");

        int r;
        r = closedir(dirp);
        if (r)
            fatal("test 002 failed");
    }

    /* check if figures dir is present */
    {
        DIR * dirp;
        dprintn("trying to open %s", figures_path);
        dirp = opendir(figures_path);
        if (!dirp)
            fatal("test 003 failed");

        int r;
        r = closedir(dirp);
        if (r)
            fatal("test 004 failed");
    }

    /* check if ifaces dir is present */
    {
        DIR * dirp;
        dprintn("trying to open %s", ifaces_path);
        dirp = opendir(ifaces_path);
        if (!dirp)
            fatal("test 005 failed");

        int r;
        r = closedir(dirp);
        if (r)
            fatal("test 006 failed");
    }

    /* check version file */
    {
        /* open read only */
        int fd;
        fd = open(version_path, O_RDONLY);
        if (fd == -1)
            fatal("test 007 failed");

        /* read and compare to real version */
        size_t bufsz;
        bufsz = strlen(NETQOS_VERSION) + 1; 
        char * buf;
        buf = xmalloc(bufsz*sizeof(char));
        ssize_t nr;
        nr = read(fd, (void *)buf, bufsz);
        if (nr == -1)
            fatal("test 008 failed");
        buf[nr-1] = '\0'; /* change \n into \0 */
        dprintn("nr = %d", nr);
        dprintn("bufsz = %d", bufsz);
        if (nr != bufsz)
            error("test 009 failed");
        dprintn("buf = %s", buf);
        dprintn("NETQOS_VERSION = %s", NETQOS_VERSION);
        if (strncmp(buf, NETQOS_VERSION, bufsz))
            error("test 010 failed");

        free(buf);
        close(fd);

        /* open write only -> should fail */
        fd = open(version_path, O_WRONLY);
        if (fd == -1) {
            if (errno == EACCES)
                dprintn("no write permissions to %s", version_path);
            else
                fatal("test 011 failed");
        } else {
            dprintn("you have write permissions to %s!", version_path);
            error("test 012 failed");
        }

        /* change permissions and try to write -> should fail with EIO */
        char * cmd;
        cmd = xcalloc(PATH_MAX + strlen("chmod u+w "));
        strcat(cmd, "chmod u+w ");
        strcat(cmd, version_path);
        dprintn("calling \"%s\"", cmd);
        int r;
        r = system(cmd);
        free(cmd);
        fd = open(version_path, O_WRONLY);
        if (fd == -1)
            fatal("test 013 failed");
        ssize_t nw;
        nw = write(fd, "hello", strlen("hello"));
        if (nw != -1)
            error("test 014 failed");
        if (errno != EIO)
            error("test 015 failed");
        dprintn("write to version file returned -EIO");
        close(fd);
        cmd = xcalloc(PATH_MAX + strlen("chmod u-w "));
        strcat(cmd, "chmod u-w ");
        strcat(cmd, version_path);
        dprintn("calling \"%s\"", cmd);
        r = system(cmd);
        free(cmd);
    }

    /* check if figures/bw dir is present */
    {
        DIR * dirp;
        dprintn("trying to open %s", fig_bw_path);
        dirp = opendir(fig_bw_path);
        if (!dirp)
            fatal("test 016 failed");

        int r;
        r = closedir(dirp);
        if (r)
            fatal("test 017 failed");
    }

    /* check if figures/delay dir is present */
    {
        DIR * dirp;
        dprintn("trying to open %s", fig_delay_path);
        dirp = opendir(fig_delay_path);
        if (!dirp)
            fatal("test 018 failed");

        int r;
        r = closedir(dirp);
        if (r)
            fatal("test 019 failed");
    }

    /* check if figures/jitter dir is present */
    {
        DIR * dirp;
        dprintn("trying to open %s", fig_jitter_path);
        dirp = opendir(fig_jitter_path);
        if (!dirp)
            fatal("test 020 failed");

        int r;
        r = closedir(dirp);
        if (r)
            fatal("test 021 failed");
    }

    /* check if figures/price dir is present */
    {
        DIR * dirp;
        dprintn("trying to open %s", fig_price_path);
        dirp = opendir(fig_price_path);
        if (!dirp)
            fatal("test 022 failed");

        int r;
        r = closedir(dirp);
        if (r)
            fatal("test 023 failed");
    }

    free(netqos_path);
    free(figures_path);
    free(ifaces_path);
    free(version_path);

    fprintf(stdout, "all tests OK\n");
    return EXIT_SUCCESS;
}
