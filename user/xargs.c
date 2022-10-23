#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "kernel/fs.h"
#include "user/user.h"


int 
main(int argc, char **argv)
{
  int idx=0, st;
  char ch;
  // if we have to pass params to exec, use char * array. 
  // or you will fail to execute;
  char *cargv[MAXARG];
  char argvs[MAXARG][MAXARG];

  for(int i=0; i<MAXARG; i++)
    cargv[i] = argvs[i];

  for(int i=1;i<argc;i++)
    strcpy(cargv[i-1], argv[i]);

  st = argc-1;
  while(read(0, &ch, sizeof(ch))) {
    if(ch=='\n') {
      cargv[st][idx]='\0';
      cargv[st+1]=0;
      idx=0;
      if(fork()==0) {
        exec(cargv[0], (char **)cargv);
        break;
      } else {
        wait(0);
      }
    } else {
      cargv[st][idx++] = ch;
    }
  }

  exit(0);
}