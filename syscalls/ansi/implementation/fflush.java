public static final int fflush(int fp)
{
  CibylFile f = (CibylFile)CRunTime.getRegisteredObject(fp);

  try {
    if (f.outputStream != null)
      f.outputStream.flush();
  } catch(IOException e) {
  }

  return 0;
}
