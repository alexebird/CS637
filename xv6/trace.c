#include "types.h"
#include "stat.h"
#include "user.h"
#include "trace.h"

int stdout = 1;

int
main(int argc, char *argv[])
{
  if (argc != 2) {
    printf(1, "Usage: trace on|off|status\n");
    exit();
  }

  char *cmd = argv[1];

  if (strcmp(cmd, "on") == 0) {
    schtrace(TRACE_ON);
    printf(1, "Scheduler tracing has been turned on.\n");
  }
  else if (strcmp(cmd, "off") == 0) {
    schtrace(TRACE_OFF);
    printf(1, "Scheduler tracing has been turned off.\n");
  }
  else if (strcmp(cmd, "status") == 0) {
    int st = schtrace(TRACE_STATUS);
    printf(1, "Scheduler tracing status: %s.\n", st == TRACE_ON ? "on" : "off");
  }
  else {
    printf(1, "Usage: trace on|off|status\n");
  }

  exit();
}
