/* My Re-Make of cat coreutil */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#define NUMNE    1		/* -b number non empty output lines */
#define NUMALL   2		/* -n number all output lines */
#define DISPEND  4		/* -E display $ at end of each line */
#define SEMPTY   8		/* -s supress repeated empty output lines */

#define MAXFILES 200
#define MAXLINE 1000

const char *progname;
char o_flags;

void concat(const char *);
void numline(char *, int *line);
int  isempty(const char *);

int
main(int argc, char *argv[])
{
  progname = argv[0];

  char c;
  while ((c = getopt(argc, argv, "bnEs")) != -1)
    {
      switch (c)
	{
	case 'b':
	  o_flags |= NUMNE;
	  break;
	case 'n':
	  o_flags |= NUMALL;
	  break;
	case 'E':
	  o_flags |= DISPEND;
	  break;
	case 's':
	  o_flags |= SEMPTY;
	  break;
	case '?':
	  return 1;
	default:
	  abort();
	}
    }

  int i;
  for (i = optind; i < argc && i < MAXFILES; i++)
    concat(argv[i]);

  return 0;
}

void
concat(const char *in)
{
  FILE *fd;

  if ((fd = fopen(in, "r")) == NULL)
    {
      perror(progname);
      exit(1);
    }

  char buf[MAXLINE];
  int line = 1;
  int prevempty = 0;

  while (fgets(buf, MAXLINE - 5, fd) != NULL)
    {
      if (o_flags & SEMPTY && prevempty && isempty(buf))
	continue;

      prevempty = isempty(buf);

      if (o_flags & NUMNE && o_flags & NUMALL)	/* Use the more restrictive numbering option */
	o_flags &= ~NUMALL;

      if (o_flags & NUMNE && !isempty(buf))
	numline(buf, &line);

      if (o_flags & NUMALL)
	numline(buf, &line);

      if (o_flags & DISPEND)
	{
	  size_t ln = strlen(buf) - 1;
	  if (buf[ln] == '\n')
	    buf[ln] = '\0';
	  strncat(buf, "$\n", MAXLINE - 2);
	}
      fputs(buf, stdout);
    }
  fclose(fd);
}

void
numline(char *buf, int *line)
{
  char pre[MAXLINE];
  sprintf(pre, "%5d  ", (*line)++);
  strncat(pre, buf, MAXLINE - 5);
  strcpy(buf, pre);
}

int
isempty(const char *buf)
{
  int i;
  for (i = 0; buf[i]; i++)
    if (buf[i] != '\n' && buf[i] != ' ' && buf[i] != '\t')
      return 0;
  return 1;
}
