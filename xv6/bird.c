#include "types.h"
#include "stat.h"
#include "user.h"

int stdout = 1;

int
main(int argc, char *argv[])
{
  printf(1, "running the bird system call.\n");
  printf(1, "the bird system call returned: %d\n", bird());
  exit();
}
