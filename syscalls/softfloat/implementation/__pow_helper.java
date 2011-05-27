public static void __pow_helper(int address_res, int address_A, int address_B)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  double b = Double.longBitsToDouble( CRunTime.memoryReadLong(address_B) );

#if defined(NOJ2ME)
  double out = Math.pow(a, b);
#else
  /* Not available */
  double out = 0;
  System.err.println("Using pow, not available in midp");
#endif /* NOJ2ME */

  CRunTime.memoryWriteLong(address_res, Double.doubleToLongBits(out));
}
