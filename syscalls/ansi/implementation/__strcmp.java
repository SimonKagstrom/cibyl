public static final int __strcmp(int s1, int s2)
{
  return Syscalls.__strncmp(s1,s2, Integer.MAX_VALUE);
}
