public static int __isnan_helper(int address_A)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  int out = Double.isNaN(a) ? 1 : 0;

  return out;
}
