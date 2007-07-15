public static final int __memset(int s, int c, int n)
{
  c = c & 0xff;

  /* GCC will optimize the word-sized memsets */
  while (((s & 0x3) != 0) && (n > 0)) {
    CRunTime.memoryWriteByte(s, c);
    s++;
    n--;
    if (n == 0) return s;
  }

  int i = 0;
  for (int j=0; j<4; j++) {
    i = i << 8;
    i |= c;
  }

  while (n > 3) {
    CRunTime.memory[s >> 2] = i;
    s += 4;
    n -= 4;
  }

  while (n > 0) {
    CRunTime.memoryWriteByte(s, c);
    s++;
    n--;
  }

  return s;
}
