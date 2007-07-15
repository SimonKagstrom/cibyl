public static void __adddf3_helper(int address_res, int address_A, int address_B)
{
  double a = Double.longBitsToDouble( CRunTime.memoryReadLong(address_A) );
  double b = Double.longBitsToDouble( CRunTime.memoryReadLong(address_B) );
  double out = a + b;

  CRunTime.memoryWriteLong(address_res, Double.doubleToLongBits(out));
}
