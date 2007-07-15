public static void __sqrt_helper(int address_res, int address_A)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  double out = Math.sqrt(a);

  CRunTime.memoryWriteLong(address_res, Double.doubleToLongBits(out));
}
