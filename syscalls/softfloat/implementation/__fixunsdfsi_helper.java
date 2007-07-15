public static int __fixunsdfsi_helper(int address_A)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  long tmp;

  /* Negative values all become zero */
  if (a < 0)
    return 0;

  /* First convert to a long */
  tmp = (long)a;

  /* If the long is too large to represent in an integer, return -1
   * which is the maximum value possible.
   */
  if (tmp > 0xffffffffl)
    return -1;

  /* Parse bits as an int */
  return (int)(tmp & 0xffffffffl);
}
