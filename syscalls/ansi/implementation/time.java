/* This will be a problem 2038 */
public static final int time(int ptr)
{
  int time = (int)(System.currentTimeMillis() / 1000);

  if (ptr != 0)
    CRunTime.memoryWriteWord(ptr, time);

  return time;
}
