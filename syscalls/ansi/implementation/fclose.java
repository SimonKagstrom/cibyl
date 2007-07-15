public static final int fclose(int fp)
{
  CibylFile f = (CibylFile)CRunTime.getRegisteredObject(fp);

  try {
    if (f.inputStream != null)
	f.inputStream.close();
    if (f.outputStream != null)
	f.outputStream.close();
#if defined(JSR075)
    if (f.fc != null)
	f.fc.close();
#endif /* JSR075 */
  } catch(Exception e) {}

  Object o = CRunTime.deRegisterObject(fp);

  return 0;
}
