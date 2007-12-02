public static void __pow_helper(int address_res, int address_A, int address_B)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  double b = Double.longBitsToDouble( CRunTime.memoryReadLong(address_B) );
  double out = 0;//Math.pow(a, b);

  /* Not available */
  System.err.println("Using pow, not available in midp");
  CRunTime.memoryWriteLong(address_res, Double.doubleToLongBits(out));
}
