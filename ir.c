

#include <stdio.h>
int
main()
{
  int    i = 0;
  while (i <= 127)
  {
    if (i <= 32)
      {
        printf("%c", '.');
      }
    printf("%d %c \n", i, i);
    i++;
  }
  return 0;
}
