#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/fs.h"

char *basename(char *path) {
  char *p;

  for(p=path+strlen(path); p >= path && *p != '/'; p--);
  p++;

  return p;
}

int match(char*, char*);

void 
find(char *path, char *pattern)
{
  int fd;
  char buf[512];
  struct stat st;
  char *filename, *p;
  struct dirent de;
  // int len;

  // printf("path: %s, strlen(path) = %d\n", path, strlen(path));
  if((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    exit(1);
  }

  if(fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }
  
  switch (st.type)
  {
  case T_DEVICE:
  case T_FILE:
    if(match(pattern, basename(path))) {
      printf("%s\n", path);
    }
    break;
  case T_DIR: 
    filename = basename(path);
    if(match(pattern, filename)) {
      printf("%s\n", path);
    }

    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      printf("find: path too long to print\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++='/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)) {
      if(de.inum == 0 || !strcmp(".", de.name) || !strcmp("..", de.name))  // why, the design of inum ?
        continue; 
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0) {
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      find(buf, pattern);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  char *path, *pattern;

  if(argc < 2) {
    fprintf(2, "Usage: find directory [pattern]\n");
    exit(1);
  }

  path = argv[1];

  if(argc >= 3) {
    pattern = argv[2];
    
    if(argc > 3)
      printf("args %s not used in find\n", argv[3]);
  } else {
    pattern = "*\0";
  }

  find(path, pattern);

  exit(0);
}

int match(char *re, char *text) {
  return !strcmp(re, text);
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

int matchhere(char*, char*);
int matchstar(int, char*, char*);

// /// @brief This match is for grep, which will check if re appears in text
// int
// match(char *re, char *text)
// {
//   if(re[0] == '^')
//     return matchhere(re+1, text);
//   do{  // must look at empty string
//     if(matchhere(re, text))
//       return 1;
//   }while(*text++ != '\0');
//   return 0;
// }

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*') {
    return matchstar(re[0], re+2, text);
  }
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}

