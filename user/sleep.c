#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int delay, nonzero, ret;

  if(argc < 2){
    fprintf(2, "Usage: sleep NUMBER...\n");
    exit(1);
  }

  if (!strcmp(argv[1], "0"))
    nonzero = 0;
  else 
    nonzero = 1;

  delay = atoi(argv[1]);
  if (delay == 0 && nonzero == 1) {
    fprintf(2, "Usage: sleep NUMBER, and NUMBER have to be a valid num\n");
    exit(2);
  }

  ret = sleep(delay);
  if (ret == -1) {
    fprintf(2, "failed to sleep for %d\n", delay);
    exit(-1);
  }

  exit(0);
}
