public static void __log10_helper(int address_res, int address_A)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );

#if defined(NOJ2ME)
  double out = Math.log10(a);
#else
  /* Not available */
  double out = 0;
  System.err.println("Using log10, not available in midp");
#endif /* NOJ2ME */

  CRunTime.memoryWriteLong(address_res, Double.doubleToLongBits(out));
}
