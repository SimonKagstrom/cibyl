public static int __fixdfsi_helper(int address)
{
  long a = CRunTime.memoryReadLong(address);

  return (int)Double.longBitsToDouble(a);
}
