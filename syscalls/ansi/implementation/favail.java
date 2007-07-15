public static final int favail(int fp)
{
    CibylFile f = (CibylFile)CRunTime.getRegisteredObject(fp);

    try
	{
#if defined(JSR075)
	    if (f.fc != null)
		return (int)f.fc.fileSize();
	    else
#endif /* JSR075 */
		return f.inputStream.available();

	} catch(Exception e) { return 0; }
}
