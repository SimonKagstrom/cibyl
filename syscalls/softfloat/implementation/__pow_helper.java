public static void __pow_helper(int address_res, int address_A, int address_B)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  double b = Double.longBitsToDouble( CRunTime.memoryReadLong(address_B) );
  double out = Math.pow(a, b);

  CRunTime.memoryWriteLong(address_res, Double.doubleToLongBits(out));
}
