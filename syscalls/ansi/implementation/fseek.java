private static final int SEEK_SET = 0;
private static final int SEEK_CUR = 1;

public static final int fseek(int fp, int offset, int whence)
{
  CibylFile f = (CibylFile)CRunTime.getRegisteredObject(fp);

  /* Only support SEEK_SET and SEEK_CUR for now */
  boolean reset = false;
  switch (whence) {
  case SEEK_SET:
    if (f.offset > offset) {
      reset = true;
      f.offset = offset;
    } else {
      offset = offset - f.offset;
      f.offset += offset;
    }
    break;
  case SEEK_CUR:
    f.offset += offset;
    break;
  default:
    return -1;
  }

  /* Only the input stream can be reset */
  if (f.inputStream != null)
    {
      try {
	if (reset) {
#if defined(JSR075)
	  if (f.fc != null) {
	    f.fc.close();
	    f.fc = (FileConnection)Connector.open(f.path);
	    f.inputStream = f.fc.openDataInputStream();

	  } else {
	    f.inputStream.reset();
	    f.inputStream.mark(f.inputStream.available());
	  }
#else
	  f.inputStream.reset();
	  f.inputStream.mark(f.inputStream.available());
#endif /* JSR075 */
	}

	f.inputStream.skip((long)offset);
	return 0;
      }
      catch(IOException e)
	{
	  return -1;
	}
    }
  return 0;
}
