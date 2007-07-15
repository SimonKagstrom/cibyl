public static void __floatsidf_helper(int address, int A)
{
  double out = (double)A;

  CRunTime.memoryWriteLong(address, Double.doubleToLongBits(out));
}
