public static int __powf_helper(int _a, int _b)
{
  float a = Float.intBitsToFloat(_a);
  float b = Float.intBitsToFloat(_b);

  /* Not available */
  System.err.println("Using pow, not available in midp");
  return 0;
}
