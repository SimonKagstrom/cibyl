public static void __atan2_helper(int address_res, int address_A, int address_B)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  double b = Double.longBitsToDouble( CRunTime.memoryReadLong(address_B) );
  double out = mMath.atan2(a, b);

  CRunTime.memoryWriteLong(address_res, Double.doubleToLongBits(out));
}
