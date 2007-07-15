public static int __cmpsf2_helper(int A, int B)
{
  float a = Float.intBitsToFloat(A);
  float b = Float.intBitsToFloat(B);

  /* see __cmpdf2_helper.java */

  if ( a < b )
    return -1;
  if ( a == b )
    return 0;
  if (a > b )
    return 1;

  /* NAN */
  return 1;

  /* Possible?: return a - b; */
}
