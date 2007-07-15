public static final int __fputs(int ptr, int fp)
{
  int count = 0;

  for (count = 0; true; count++)
    {
      int c = CRunTime.memoryReadByte(ptr + count);

      if (c == '\0')
	break;

      if (Syscalls.fputc(c, fp) == -1)
	return -1;
    }

  return count;
}
