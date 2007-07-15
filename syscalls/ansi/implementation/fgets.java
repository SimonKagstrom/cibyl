public static final int fgets(int ptr, int size, int fp)
{
  int s = 0;

  while (s < size)
    {
      int c = Syscalls.fgetc(fp);

      if (Syscalls.ferror(fp) != 0)
	return 0;

      CRunTime.memoryWriteByte(ptr + s, c);

      if (c == '\n' || c == -1) {
	// feof is not working
        if ((s == 0) && (c == -1)) return 0;

	return ptr;
      }

      s++;
    }

  return ptr;
}
