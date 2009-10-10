#include "types.h"
#include "stat.h"
#include "user.h"

int stdout = 1;

int
main(int argc, char *argv[])
{
  if (argc < 2 || argc > 3) {
    printf(stdout, "Usage: tix PID TICKETS\n");
    exit();
  }

  int pid = atoi(argv[1]);
  if (pid < 1) {
    printf(stdout, "PID must be >= 1.\n");
    exit();
  }

  if (argc == 3) {
    int tickets = atoi(argv[2]);
    if (tickets < 0) {
      printf(stdout, "TICKETS must be >= 0.\n");
      exit();
    }

    settix(pid, tickets);
  }
  else if (argc == 2) {
    int tix = gettix(pid);
    if (tix != -1)
      printf(stdout, "Tickets held by job %d: %d\n", pid, tix);
    else
      printf(stdout, "Job %d was not found.\n", pid);

  }
  exit();
}
