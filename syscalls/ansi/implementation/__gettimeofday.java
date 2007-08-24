/* This will be a problem 2038 */
public static final int __gettimeofday(int ptr, int ignored)
{
  long tm = System.currentTimeMillis();
  int secs = (int)(tm / 1000);
  int usecs = (int)((tm % 1000) * 1000);

  if (ptr != 0)
    {
      CRunTime.memoryWriteWord(ptr, secs);
      CRunTime.memoryWriteWord(ptr+4, usecs);
    }

  return 0;
}
