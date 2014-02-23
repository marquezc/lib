#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>


#define MAXFILENAME 255

int
main(int argc, char *argv[])
{
  DIR   *dp;
  struct dirent *dirent;

  if (argc > 2)
    {
      printf("Usage: argument (dir) req'd\n");
      exit(1);
    }

  char   buf[MAXFILENAME];

  if ((dp = opendir(getcwd(buf, MAXFILENAME))) == NULL)
    {
      printf("Error: couldn't open %s\n", argv[1]);
      exit(1);
    }
}
