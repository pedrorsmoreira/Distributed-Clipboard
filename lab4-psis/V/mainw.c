#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    int fd;

    /* create the FIFO (named pipe) */
    mkfifo("fifo", 0666);

    /* write "Hi" to the FIFO */
    fd = open("fifo", O_WRONLY);
    write(fd, "testetetet", sizeof("testetetet"));
    close(fd);

    /* remove the FIFO */
    unlink("fifo.txt");

    return 0;
}