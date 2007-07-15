public static final int __strcpy(int dest, int src)
{
  return Syscalls.__strncpy(dest, src, Integer.MAX_VALUE);
}
