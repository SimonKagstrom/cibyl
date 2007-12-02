public static int __log10f_helper(int _a)
{
  float a = Float.intBitsToFloat(_a);

  /* Not available */
  System.err.println("Using log10, not available in midp");
  return 0; //Float.floatToIntBits( (float)Math.log10((double)a) );
}
