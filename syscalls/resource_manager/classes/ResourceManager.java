/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      ResourceManager.java
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Resource manager (images etc)
 *
 * $Id: ResourceManager.java 13523 2007-02-09 17:17:52Z ska $
 *
 ********************************************************************/
import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;
import java.util.*;
import java.io.*;

public class ResourceManager
{
  private Hashtable images;
  private boolean logActive;

  private static ResourceManager rm;

  private ResourceManager()
  {
    images = new Hashtable();
    logActive = true;
  }

  public static ResourceManager getInstance()
  {
    if (ResourceManager.rm == null)
      ResourceManager.rm = new ResourceManager();

    return ResourceManager.rm;
  }

  public void activateLog()
  {
    this.logActive = true;
  }

  public void deActivateLog()
  {
    this.logActive = false;
  }

  public void log(String str)
  {
    if (this.logActive)
      System.err.println(str);
  }


  public Image getImage(String name) throws IOException
  {
    Image image;

    if (images.get(name) != null)
      return (Image)images.get(name);

    try {
      image = Image.createImage(name);
    } catch(IOException e) {
      this.log("Could not open image" + name);
      throw e;
    }

    images.put(name, image);

    return image;
  }

  public DataInputStream getResourceStream(String name) throws Exception
  {
    DataInputStream out = null;

    try
      {
	InputStream stream = this.getClass().getResourceAsStream(name);
	if (stream == null) return null;
	out = new DataInputStream(stream);
      }
    catch(Exception e)
      {
	this.log("Exception while opening " + name);
	throw e;
      }

    return out;
  }

  /* Manage other resources ... */
}
