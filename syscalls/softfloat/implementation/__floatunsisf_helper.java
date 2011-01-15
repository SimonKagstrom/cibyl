public static int __floatunsisf_helper(int i)
{
	long v = (long)i;
	
	v &= 0x00000000ffffffffl;

	return Float.floatToIntBits((float)v);
}
