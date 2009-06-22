#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BW_PATH "/sys/kernel/netqos/figures/bw/eth0"

int
main(int argc, char ** argv)
{
    /* open the bandwidth file */
    int fd;
    {
        fd = open(BW_PATH, O_RDONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
    }

    /* read bandwidth file */
    unsigned int bw;
    {
        /* hope 10 is enought space for bandwidth */
        size_t bufsz = 10;
        char buf[bufsz];

        /* read raw data and add '\0' at the end */
        ssize_t nr;
        nr = read(fd, buf, bufsz-1);
        if (nr == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (nr == 0) {
            fprintf(stderr, "%s: empty file\n", BW_PATH);
            exit(EXIT_FAILURE);
        }
        buf[nr] = '\0';

        /* change buf into a proper data type */
        int r;
        r = sscanf(buf, "%u\n", &bw);
        if (r == EOF) {
            perror("sscanf");
            exit(EXIT_FAILURE);
        }
    }

    /* print version and exit */
    {
        printf("eth0 bw = %u kbps\n", bw);

        int r;
        r = close(fd);
        if (r == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    }
}
