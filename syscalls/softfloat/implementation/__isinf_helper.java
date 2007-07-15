public static int __isinf_helper(int address_A)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  int out = Double.isInfinite(a) ? 1 : 0;

  return out;
}
