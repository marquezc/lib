#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXFILENAME			255
char		*filename;

int
main(int argc, char *argv[])
{
  DIR						*dp;
  struct dirent *dirent;

  if (argc > 2)
    {
      printf("Usage: argument (dir) req'd\n");
      exit(1);
    }

	filename = (argc == 1) ? getcwd(filename, MAXFILENAME) : argv[1];
	if ((dp = opendir(filename)) == NULL)
    {
      printf("Error: couldn't open %s\n", argv[1]);
      exit(1);
    }

	while ((dirent = readdir(dp)) != NULL)
    if (dirent->d_name[0] != '.')
      printf("%s\n", dirent->d_name);
}
