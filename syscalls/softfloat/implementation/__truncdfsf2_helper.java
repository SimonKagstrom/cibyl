public static int __truncdfsf2_helper(int address)
{
  float out = (float)Double.longBitsToDouble( CRunTime.memoryReadLong(address) );

  return Float.floatToIntBits(out);
}
