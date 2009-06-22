#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define BW_PATH "/sys/kernel/netqos/figures/bw/eth0"
#define NEW_BW 123

int
main (int argc, char ** argv)
{
    /* open bw file */
    int fd;
    {
        fd = open(BW_PATH, O_WRONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
    }

    /* write the new bw into the bw file */
    {
        size_t bufsz = 10; /* hope its enough for even big bws */
        char buf[bufsz];

        /* change the bw into a string */
        int nc;
        nc = snprintf(buf, bufsz, "%d\n", NEW_BW);
        if (nc < 0) {
            perror("sprintf");
            exit(EXIT_FAILURE);
        }
        if (nc >= bufsz) {
            fprintf(stderr, "buffer is too small\n");
            exit(EXIT_FAILURE);
        }

        /* write the string */
        ssize_t nw;
        nw = write(fd, buf, nc+1);
        if (nw == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    /* print, close & exit */
    {
        printf("eth0 bw changed to %u kbps\n", NEW_BW);
        
        int r;
        r = close(fd);
        if (r == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }
        
        exit(EXIT_SUCCESS);
    }
}
