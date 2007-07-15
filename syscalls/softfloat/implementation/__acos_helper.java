public static void __acos_helper(int address_res, int address_A)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  double out = mMath.acos(a);

  CRunTime.memoryWriteLong(address_res, Double.doubleToLongBits(out));
}
