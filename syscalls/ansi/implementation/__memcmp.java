public static final int __memcmp(int s1, int s2, int n)
{
  int s = s1;
  int d = s2;
  int a = 0;
  int b = 0;

  do
    {
      if (s - s1 >= n)
	break;

      a = CRunTime.memoryReadByte(s);
      b = CRunTime.memoryReadByte(d);

      s++;
      d++;
    } while ( a == b );

  if (a < b)
    return -1;
  else if (a > b)
    return 1;
  return 0;
}
