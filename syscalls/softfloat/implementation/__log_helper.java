public static void __log_helper(int address_res, int address_A)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  double out = 0; //Math.log(a);

  /* Not available */
  System.err.println("Using log, not available in midp");
  CRunTime.memoryWriteLong(address_res, Double.doubleToLongBits(out));
}
