public static final int fopen(int __path, int __mode)
{
  String path = CRunTime.charPtrToString(__path);
  String mode = CRunTime.charPtrToString(__mode);
  int out = 0;

  CibylFile f = new CibylFile(path);

  if (mode.indexOf("r") != -1)
    {
      try
	{
#if defined(NOJ2ME)
 	  FileInputStream fileIs = new FileInputStream(path);
 	  DataInputStream is = new DataInputStream( (InputStream)fileIs );
#else
	  DataInputStream is = ResourceManager.getInstance().getResourceStream(path);
#endif /* NOJ2ME */

	  if (is == null)
	    return 0;

	  f.inputStream = is;
	} catch(Exception e) { return 0; }
    }
  else
    {
      System.err.println("Mode '" + mode + "' not supported");
      return 0;
    }

  return CRunTime.registerObject(f);
}
