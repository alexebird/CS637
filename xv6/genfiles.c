#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

int stdout = 1;
char buf[2048];
char name[6];

void writetest(char *name)
{
    int fd = open(name, O_CREATE|O_RDWR);

    if(fd < 0) {
        printf(stdout, "error: creat small failed!\n");
        exit();
    }

    int i;
    for(i = 0; i < 225; i++) {  // 8 blocks per file. (8th block has only 1 byte)
        if(write(fd, "aaaaaaaa", 8) != 8) {
            printf(stdout, "error: write aa %d new file failed\n", i);
            exit();
        }
        if(write(fd, "bbbbbbbb", 8) != 8) {
            printf(stdout, "error: write bb %d new file failed\n", i);
            exit();
        }
    }

    close(fd);


    fd = open(name, O_RDONLY);

    if(fd < 0) {
        printf(stdout, "error: open small failed!\n");
        exit();
    }

    i = read(fd, buf, 2000);

    if(i != 2000) {
        printf(stdout, "read failed\n");
        exit();
    }

    close(fd);
}

char *nextfname()
{
    //name[3]++;

    if (++name[3] == 0x5b) {
        name[3] = 0x41;
        //name[2]++;
        if (++name[2] == 0x5b) {
            name[2] = 0x41;
            name[1]++;
        }
    }

    return name;
}

void createtest()
{
    int i, fd;
    int numfiles = 100;

    printf(stdout, "STARTING: many creates test\n");

    for(i = 0; i < numfiles; i++) {
        //printf(stdout, "file %d: %s\n", i + 1, nextfname());
        writetest(nextfname());
    }

    printf(stdout, "STARTING: unlinking\n");
    name[0] = 'f';
    name[1] = 'A';
    name[2] = 'A';
    name[3] = 'A' - 1;
    name[4] = '\0';

    for(i = 0; i < numfiles; i++) {
        //printf(stdout, "unlinking %d: %s\n", i + 1, nextfname());
        if(unlink(nextfname()) < 0) {
            printf(stdout, "unlink failed\n");
            exit();
        }
    }
    printf(stdout, "PASSED: many creates, writes, and reads\n");
}

int main(int argc, char *argv[])
{
    printf(stdout, "Starting genfiles.\n");

    name[0] = 'f';
    name[1] = 'A';
    name[2] = 'A';
    name[3] = 'A' - 1;
    name[4] = '\0';

    createtest();
    printf(stdout, "Exiting genfiles.\n");
    exit();
}

