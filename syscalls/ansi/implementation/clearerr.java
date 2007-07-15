public static final void clearerr(int fp)
{
  CibylFile f = (CibylFile)CRunTime.getRegisteredObject(fp);

  f.eof = 0;
  f.error = 0;
}
