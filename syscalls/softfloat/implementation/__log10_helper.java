public static void __log10_helper(int address_res, int address_A)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  double out = Math.log10(a);

  CRunTime.memoryWriteLong(address_res, Double.doubleToLongBits(out));
}
