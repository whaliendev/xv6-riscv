#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

typedef unsigned char byte;

int
main(int argc, char *argv[])
{
  byte* v = (byte *)"7";
  int pc[2], cp[2];
  
  pipe(pc);
  pipe(cp);
  if (fork() == 0) {
    int ret;
    byte buf[2] = {'0'};

    ret = read(pc[0], buf, 1);
    if (ret == -1) {
        fprintf(2, "pingpong fail to read from pipe\n");
        exit(-1);
    }

    printf("%d: received ping\n", getpid());

    ret = write(cp[1], buf, 1);
    if (ret == -1) {
        fprintf(2, "pingpong fail to write to pipe\n");
        exit(-1);
    }
    
  } else {
    int ret; 

    ret = write(pc[1], v, 1);
    if (ret == -1) {
        fprintf(2, "pingpong fail to write to pipe\n");
        exit(-1);
    }

    ret = read(cp[0], v, 1);
    if (ret == -1) {
        fprintf(2, "pingpong fail to read from pipe\n");
        exit(-1);
    }
    
    printf("%d: received pong\n", getpid());

    wait(0);
  }

  exit(0);
}
