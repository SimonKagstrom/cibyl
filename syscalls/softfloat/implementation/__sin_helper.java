public static void __sin_helper(int address_res, int address_A)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  double out = Math.sin(a);

  CRunTime.memoryWriteLong(address_res, Double.doubleToLongBits(out));
}
