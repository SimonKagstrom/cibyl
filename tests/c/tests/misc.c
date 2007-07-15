#error This does not compile yet...

void test_malloc(void)
{
  void *allocated_areas[8];
  int i, j;

  printf("Running test_malloc:\n  ");

  for (i = 0; i < 8; i++)
    {
      int *area = malloc(sizeof(int) * 8);
      if (area == NULL)
	{
	  printf("TEST FAILURE: malloc\n");
	  return;
	}

      for (j=0; j < 8; j++)
	area[j] = i+1;
      allocated_areas[i] = area;
    }

  for (i = 0; i < 8; i += 2)
    {
      int *area = allocated_areas[i];

      for (j = 0; j < 8; j++)
	{
	  if (area[j] != i+1)
	    {
	      printf("TEST FAILURE: assert: ");
	      int_to_hex(j);
	      printf(" Should be ");
	      int_to_hex(i+1);
	      printf(" is ");
	      int_to_hex(area[j]);
	      return;
	    }
	}
      free(area);
    }
  for (i = 1; i < 8; i += 2)
    {
      int *area = allocated_areas[i];

      for (j = 0; j < 8; j++)
	{
	  if (area[j] != i+1)
	    {
	      printf("TEST FAILURE: assert2\n");
	      return;
	    }
	}
      free(area);
    }
  printf("succeded: malloc\n");
}

void test_strcpy(char *a)
{
  char buf[255];
  printf("Running test_strcpy: %s\n", a);
  strcpy(buf, a);
  if (strncmp(buf, a, 255) != 0) {
      printf("TEST_FAILURE: ");
    }
  else
    printf("succeeded: %s\n", buf);
}

void test_strcat(char *a, char *b, char *answer)
{
  puts("Running test_strcat: "); puts(a); puts(","); puts(b); puts("\n  ");
  strcat(a, b);
  if (strncmp(a, answer, 255) != 0) {
      puts("TEST_FAILURE: ");
    }
  else
    puts("succeeded: ");
  puts(a); puts("\n");
}

void test_strcmp(char *a, char *b, int answer)
{
  int out = strcmp(a, b);
  puts("Running test_strcmp: "); puts(a); puts(","); puts(b); puts("\n  ");
  if (out != answer) {
      puts("TEST_FAILURE: ");
    }
  else
    puts("succeeded: ");
  int_to_hex(out); puts(","); int_to_hex(answer); puts("\n");
}

void test_strlen(char *a, int answer)
{
  int len = strlen(a);

  puts("Running test_strlen: "); puts(a); puts("\n  ");
  if (len != answer) {
      puts("TEST_FAILURE: ");
    }
  else
    puts("succeeded: ");
  int_to_hex(len); puts("\n");
}
