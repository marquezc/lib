#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *progname = "test";
static char *filename;

int    cext(char *);

#define MAXLINE 80
#define USGERR printf("Usage: %s filename.c\n", progname);

struct dtype
{
  char  *tag;
  int    cnt;
} dtypes[] =
{
  {
  " char ", 0},
  {
  " short ", 0},
  {
  " int ", 0},
  {
  " long ", 0},
  {
  " double ", 0},
  {
  " float ", 0}
};

int
main(int argc, char *argv[])
{
  if (argc != 2 || !cext(argv[1]))
    {
      USGERR;
      exit(1);
    }

  filename = argv[1];

  FILE  *fp = fopen(filename, "r");
  char   buf[MAXLINE];

  while (fgets(buf, MAXLINE, fp))
    {
      int    i;
      for (i = 0; i < 6; i++)
	{
	  struct dtype *dtype = &dtypes[i];
	  if (strstr(buf, dtype->tag))
	    fputs(buf, stdout);
	}
    }
}

int
cext(char *name)
{
  int    x = strlen(name) - 2;
  return strstr(name + x, ".c") ? 1 : 0;
}
