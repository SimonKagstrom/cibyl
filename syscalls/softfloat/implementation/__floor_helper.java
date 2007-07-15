public static void __floor_helper(int address_res, int address_A)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  double out = Math.floor(a);

  CRunTime.memoryWriteLong(address_res, Double.doubleToLongBits(out));
}
