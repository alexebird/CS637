#include "types.h"
#include "stat.h"
#include "user.h"

int stdout = 1;

int
main(int argc, char *argv[])
{
  printf(1, "Forking four jobs.\n");

  int pid;
  int i;
  for(i = 0; i < 4; i++) {
    pid = fork();
    if (pid == 0) {
      printf(1, "Child pid: %d\n", getpid());
      for (;;)
        ;//spin
    }
    else if (pid < 0)
      printf(1, "fork() error.\n");
  }

  exit();
}
