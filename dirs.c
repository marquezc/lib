#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>


int
main (int argc, char *argv[])
{
  DIR *dp;
  struct dirent *dirent;

  if (argc != 2) 
    {
      printf("Usage: argument (dir) req'd\n");
      exit(1);
    }
  
  if ((dp = opendir(argv[1])) == NULL)
    {
      printf("Error: couldn't open %s\n", argv[1]);
      exit(1);
    }

  char s;
  s = get_current_dir_name();

  printf("%s\n", s);
}
