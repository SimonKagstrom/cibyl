public static void __atan_helper(int address_res, int address_A)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  double out = mMath.atan(a);

  CRunTime.memoryWriteLong(address_res, Double.doubleToLongBits(out));
}
