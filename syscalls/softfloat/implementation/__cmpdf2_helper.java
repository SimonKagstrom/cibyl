public static int __cmpdf2_helper(int address_A, int address_B)
{
  double a = Double.longBitsToDouble(CRunTime.memoryReadLong(address_A));
  double b = Double.longBitsToDouble(CRunTime.memoryReadLong(address_B));

  /*
   * From the gccint info page:
   *
   *   These functions calculate a <=> b.  That is, if A is less than
   *   B, they return -1; if A is greater than B, they return 1; and
   *   if A and B are equal they return 0.  If either argument is NaN
   *   they return 1, but you should not rely on this; if NaN is a
   *   possibility, use one of the higher-level comparison functions.
   */

  if ( a < b )
    return -1;
  if ( a == b )
    return 0;
  if (a > b )
    return 1;

  /* NAN */
  return 1;

  /* Possible?: return a - b; */
}
