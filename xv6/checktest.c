#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int stdout = 1;

int
main(int argc, char *argv[])
{
  printf(stdout, "Testing the check() system call.\n");

  int fd = open("/README", O_RDONLY);
  if(fd < 0){
    printf(stdout, "open() failed.\n");
    exit();
  }

  int offset = 300;
  printf(stdout, "after open(): %d\n", check(fd, offset));

  printf(stdout, "before read(): %d\n", check(fd, offset));
  #define RDSIZE (256)
  char *buffer[RDSIZE];
  int err;
  if ((err = read(fd, buffer, RDSIZE)) < 0) {
    printf(stdout, "read() failed.\n");
    exit();
  }
  printf(stdout, "after read(): %d\n", check(fd, offset));

  printf(stdout, "buffer:\n%s\n", buffer);

  close(fd);

  exit();
}

