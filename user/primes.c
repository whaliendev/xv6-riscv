#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

const static int start = 2, end = 35;

void
source() {
  for(int n=start; n<=end;n++) {
    write(1, &n, sizeof(n));
  }
}

void
cull(int p) {
  int n;
  while(read(0, &n, sizeof(n))) {
    if(n%p!=0)  {
      write(1, &n, sizeof(n));
    }
  }
}

void
redirect(int k, int pd[2]) {
  close(k);
  dup(pd[k]);
  close(pd[0]);
  close(pd[1]);
}

void
sink() {
  int pd[2];
  int p;
  while(read(0, &p, sizeof(p))) {
    printf("prime %d\n", p);
    pipe(pd);
    // Note that there is magic: when forking from parent, the child process
    // inherits its stdin, which is the read end of last pipe output. That is
    // to say, the former child process filters the multiplies of its prime
    // and pipe the rest nums to the latter child process. 
    if(fork()) {
      redirect(0,pd);
    } else {
      redirect(1, pd);
      cull(p);
    }
  }
}

/// @brief a coroutine prime number sieve
/// the data flows among child processes and parent process: 
/// source -> parent -> cull(2) process -> parent -> cull(3) process -> ...
/// To some extent, I believe it's magic
/// Ref: 
///    1. https://www.cs.dartmouth.edu/~doug/sieve/sieve.pdf
///    2. https://swtch.com/~rsc/thread
int
main(int argc, char *argv[]) {
  int pd[2];
  pipe(pd);
  if(fork()) { // parent process
    redirect(0, pd);  // connect stdin to pipe read end
    sink();
  } else {
    redirect(1, pd);  // connect stdout to pipe write end
    source();
  }

  // Notice that the main process is always the last process to exit, as its
  // `read` syscall will return when every else child process exit 0. 

  exit(0);
}



