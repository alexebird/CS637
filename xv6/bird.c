#include "types.h"
#include "stat.h"
#include "user.h"

int stdout = 1;

int
main(int argc, char *argv[])
{
  printf(stdout, "running the bird system call.\n");
  printf(stdout, "the bird system call returned: %d\n", bird());
  printf(stdout, "clock ticks: %d\n", ticks());
  exit();
}
