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
  public static void main(String args[])
  {
    int i;
    RandomAccessFile f;
    InputStream is = null;

    if (args.length >= 1) {
	System.err.println("Usage: StandaloneMain <memory-image-file>\n");
	System.err.println("The memory image is typically program.data.bin");
	System.exit(1);

	String imageFilename = args[0];
	FileInputStream fileIs = null;

	try {
	    fileIs = new FileInputStream(imageFilename);
	    is = new DataInputStream( (InputStream)fileIs );
	}
	catch (Exception exception) {
	    System.err.println("Could not open " + imageFilename + "\n");
	    System.err.println(exception);
	    System.exit(1);
	}
    }
    else {
	try {
	    is = new StandaloneMain().getClass().getResourceAsStream("/program.data.bin");
	}
	catch (Exception exception) {
	    System.err.println("Could not open program.data.bin\n");
	    System.err.println(exception);
	    System.exit(1);
	}
    }


    try {
        int start = CibylCallTable.getAddressByName("__start");

        CRunTime.init(is);
        int sp = (CRunTime.memory.length * 4) - 8;
        CRunTime.publishCallback("Cibyl.atexit"); /* Never used! */
        CibylCallTable.call(start, sp, 0, 0, 0, 0);
    } catch(Exception e)
    {
    	System.out.println(e.getMessage());
    	e.printStackTrace();
    }
  }
}
