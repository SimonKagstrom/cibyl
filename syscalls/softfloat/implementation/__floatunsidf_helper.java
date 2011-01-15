public static void __floatunsidf_helper(int address, int i)
{
	long v = (long)i;

	v &= 0x00000000ffffffffl;

	double out = (double)v;

	CRunTime.memoryWriteLong(address, Double.doubleToLongBits(out));
}
