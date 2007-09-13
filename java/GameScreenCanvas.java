/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      GameScreenCanvas.java
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Base-class for Cibyl/J2ME
 *
 * $Id: GameScreenCanvas.java 14081 2007-03-09 20:48:00Z ska $
 *
 ********************************************************************/
import javax.microedition.io.*;
import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;
import javax.microedition.lcdui.game.*;
import java.util.*;
import java.io.*;

public class GameScreenCanvas extends GameCanvas implements Runnable
{
  private Display display;
  private Main main;
  public int callbacks[];

  /* Yes, this is ugly. Bear in mind that this class is only a helper
   * for the C funtionality in Cibyl
   */
  public GameScreenCanvas(Display d, Main m)
  {
    super(CibylConfig.supressKeyEvents);

    this.display = d;
    this.main = m;

    this.callbacks = new int[10];
  }

  public void start()
  {
    Thread t = new Thread(this);
    t.start();
    display.setCurrent(this);
  }

  public void stop()
  {
  }

  private void invokeCallback(int which, int a0, int a1)
  {
    if (this.callbacks[which] != 0)
      CibylCallTable.call(this.callbacks[which],
                          CRunTime.eventStackPointer,
                          a0, a1, 0, 0); /* a0 ... a3 */
  }

  /* Callbacks */
  protected void keyPressed(int keyCode)
  {
    this.invokeCallback(0, keyCode, -1);
  }

  protected void keyReleased(int keyCode)
  {
    this.invokeCallback(1, keyCode, -1);
  }

  protected void keyRepeated(int keyCode)
  {
    this.invokeCallback(2, keyCode, -1);
  }

  protected void pointerDragged(int x, int y)
  {
    this.invokeCallback(3, x, y);
  }

  protected void pointerPressed(int x, int y)
  {
    this.invokeCallback(4, x, y);
  }

  protected void pointerReleased(int x, int y)
  {
    this.invokeCallback(5, x, y);
  }

  private DataInputStream getResourceStream(String name)
  {
    DataInputStream out = null;

    try
      {
	InputStream stream = this.getClass().getResourceAsStream(name);
	out = new DataInputStream(stream);
      }
    catch(Exception e)
      {
	System.err.println("Exception while opening " + name);
	this.main.destroyApp(true);
      }

    return out;
  }

  /* The main thread function */
  public void run()
  {
    DataInputStream is = this.getResourceStream("/program.data.bin");

    /* Setup the runtime support */
    CRunTime.init(is);
    Syscalls.initJ2ME(this, this.getGraphics(), this.main);

    try { /* Added */
      /* Start the virtual machine */
      Cibyl.start(0, /* sp, set in crt0.S */
                  0, /* a0 */
                  0, /* a1 */
                  0, /* a2 */
                  0);/* a3 */
    } catch (Exception e) { e.printStackTrace(); }
  }
}
