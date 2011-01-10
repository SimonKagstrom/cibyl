#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  char buf[80];
  const char *txt = "Hejsan\n";
  FILE *fp;

  /* Open file for writing */
  fp = fopen("tst", "w");

  if (!fp)
    {
      printf("Could not open file for output\n");
      return 1;
    }

  fwrite(txt, sizeof(char), strlen(txt) + 1, fp);
  fclose(fp);

  fp = fopen("tst", "r");
  if (!fp)
    {
      printf("Could not open file for reading\n");
      return 1;
    }
  fgets(buf, 80, fp);
  fclose(fp);

  printf("Reading from file: %s\n", buf);

  return 0;
}
