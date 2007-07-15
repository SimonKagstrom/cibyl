	public static final int fe_load_data(int a0, int a1, int a2, int a3) {
	  int outAddress = a0;
	  int offset = a1;
	  int len = a2;
	  String filename = CRunTime.charPtrToString(a3);

	  InputStream stream = null;
	  DataInputStream is = null;
	  GameCanvas gc = (GameCanvas)CRunTime.getRegisteredObject( Syscalls.canvasHandle ); /* See syscalls/j2me/init */
	  int i = 0;

	  /* Open the file */
	  try
	    {
	      Class c = gc.getClass();
	      stream = c.getResourceAsStream("/" + filename);
	      is = new DataInputStream(stream);
	    }
	  catch(Exception e)
	    {
	      System.out.println("MABPOOA\n" + e);
	      return 1;
	    }
	  /* Read the file */
	  try
	    {
	      is.skip(offset);
	      while (i < len)
		{
		  byte b = is.readByte();
		  CRunTime.memoryWriteByte(outAddress + i, b);
		  i++;
		}
	      is.close();
	    }
	  catch(Exception e)
	    {
	      System.err.println("Threw " + e);
	      return 1;
	    }

	  return 0;
	}
