#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define BSIZE 512

int stdout = 1;

int checkOffsets(int fd, int block, int shouldBeInBC)
{
  int startOffset = BSIZE * block;
  int i;
  for (i = startOffset; i < startOffset + BSIZE; i++) {
    if (check(fd, i) != shouldBeInBC)
      return 0;
  }
  
  return 1;
}

void assertBlock(int fd, int block, int assertedVal, char *msg)
{
  int passed = checkOffsets(fd, block, assertedVal);
  if (passed)
    printf(stdout, "PASSED (block=%d,asserted=%d) %s\n", block, assertedVal, msg);
  else
    printf(stdout, "FAILED (block=%d,asserted=%d) %s\n", block, assertedVal, msg);
}

int
main(int argc, char *argv[])
{
  printf(stdout, "Testing the check() system call.\n");

  int fd = open("/README", O_RDONLY);
  if(fd < 0){
    printf(stdout, "open() failed.\n");
    exit();
  }

  assertBlock(fd, 0, 0, "before read");
  assertBlock(fd, 1, 0, "before read");


  #define RDSIZE (512) // Only 1 block will be read.
  char *buffer[RDSIZE];
  int err;
  if ((err = read(fd, buffer, RDSIZE)) < 0) {
    printf(stdout, "read() failed.\n");
    exit();
  }

  assertBlock(fd, 0, 1, "after read");
  assertBlock(fd, 1, 0, "after read");

  if ((err = read(fd, buffer, RDSIZE)) < 0) {
    printf(stdout, "read() failed.\n");
    exit();
  }

  assertBlock(fd, 0, 1, "after read");
  assertBlock(fd, 1, 1, "after read");
  assertBlock(fd, 2, 0, "after read");

  close(fd);
  exit();
}

