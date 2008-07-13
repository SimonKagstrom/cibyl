/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      StandaloneMain.java
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Main class for standalone programs for Cibyl
 *
 * $Id: StandaloneMain.java 13453 2007-02-05 16:28:37Z ska $
 *
 ********************************************************************/
import java.lang.*;
import java.io.*;

public class StandaloneMain
{
  public static void initCibyl(InputStream is)
  {
    try {
        int start = CibylCallTable.getAddressByName("__start");
        int destructors = CibylCallTable.getAddressByName("crt0_run_global_destructors");

        CRunTime.init(is);
        int sp = (CRunTime.memory.length * 4) - 8;
        CRunTime.publishCallback("Cibyl.atexit"); /* Never used! */
        CibylCallTable.call(start, sp, 0, 0, 0, 0);
        CibylCallTable.call(destructors, sp, 0, 0, 0, 0);
    } catch(Exception e)
    {
    	System.out.println(e.getMessage());
    	e.printStackTrace();
    }
  }

  public static void cleanupCibyl()
  {

    try {
        int destructors = CibylCallTable.getAddressByName("crt0_run_global_destructors");

        int sp = (CRunTime.memory.length * 4) - 8;
        CibylCallTable.call(destructors, sp, 0, 0, 0, 0);
    } catch(Exception e)
    {
    	System.out.println(e.getMessage());
    	e.printStackTrace();
    }
  }


  public static void testStuff()
  {
    LibraryWrapper l = new LibraryWrapper();

    int v = l.add_to_arg(5);

    System.out.println("Called C add_to_arg, return value " + v);

    int ptr = l.get_int_pointer();
    System.out.println("Called C get_int_pointer, return value " + Integer.toHexString(ptr));

    l.update_pointer(ptr);
  }

  public static void main(String args[])
  {
    int i;
    RandomAccessFile f;
    String imageFilename = args[0];

    FileInputStream fileIs = null;
    DataInputStream is = null;

    /* Read the data section */
    try {
      fileIs = new FileInputStream(imageFilename);
      is = new DataInputStream( (InputStream)fileIs );
    }
    catch (Exception exception) {
      System.err.println("Could not open " + imageFilename + "\n");
      System.err.println(exception);
      System.exit(1);
    }

    /* init Cibyl */
    StandaloneMain.initCibyl(is);

    StandaloneMain.testStuff();

    StandaloneMain.cleanupCibyl();
  }
}
