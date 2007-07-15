public static final int feof(int fp)
{
  CibylFile f = (CibylFile)CRunTime.getRegisteredObject(fp);

  return f.eof;
}
