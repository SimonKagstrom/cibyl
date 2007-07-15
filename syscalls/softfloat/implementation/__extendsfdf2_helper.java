public static void __extendsfdf2_helper(int address, int A)
{
  double out = (double)Float.intBitsToFloat(A);

  CRunTime.memoryWriteLong(address, Double.doubleToLongBits(out));
}
