public static void __exp_helper(int address_res, int address_A)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  double out = Math.exp(a);

  CRunTime.memoryWriteLong(address_res, Double.doubleToLongBits(out));
}
