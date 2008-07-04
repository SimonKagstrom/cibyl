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

  private int cb_keyPressed, cb_keyReleased, cb_keyRepeated, cb_pointerDragged, cb_pointerPressed, cb_pointerReleased;

  /* Yes, this is ugly. Bear in mind that this class is only a helper
   * for the C funtionality in Cibyl
   */
  public GameScreenCanvas(Display d, Main m)
  {
    super(CibylConfig.supressKeyEvents);

    this.display = d;
    this.main = m;

    try {
      DataInputStream is = this.getResourceStream("/program.data.bin");

      /* Setup the runtime support */
      CRunTime.init(is);
      is.close();
      is = null;
    } catch (Exception e) {
      this.showError(e, e.getMessage());
    }

    cb_keyPressed      = CRunTime.publishCallback("javax.microedition.lcdui.game.GameCanvas.keyPressed");
    cb_keyReleased     = CRunTime.publishCallback("javax.microedition.lcdui.game.GameCanvas.keyReleased");
    cb_keyRepeated     = CRunTime.publishCallback("javax.microedition.lcdui.game.GameCanvas.keyRepeated");
    cb_pointerDragged  = CRunTime.publishCallback("javax.microedition.lcdui.game.GameCanvas.pointerDragged");
    cb_pointerPressed  = CRunTime.publishCallback("javax.microedition.lcdui.game.GameCanvas.pointerPressed");
    cb_pointerReleased = CRunTime.publishCallback("javax.microedition.lcdui.game.GameCanvas.pointerReleased");
  }

  public void start()
  {
    Thread t = new Thread(this);
    t.start();
  }

  public void stop()
  {
  }

  public void invokeCallback(int which, int a0, int a1)
  {
    try {
      CRunTime.invokeCallback(which, a0, a1, 0, 0);
    } catch(Exception e) {
      this.showError(e, "Calling " + Integer.toHexString(which) + " failed: " + e.getMessage());
    }
  }

  /* Callbacks */
  protected void keyPressed(int keyCode)
  {
    this.invokeCallback(this.cb_keyPressed, keyCode, -1);
  }

  protected void keyReleased(int keyCode)
  {
    this.invokeCallback(this.cb_keyReleased, keyCode, -1);
  }

  protected void keyRepeated(int keyCode)
  {
    this.invokeCallback(this.cb_keyRepeated, keyCode, -1);
  }

  protected void pointerDragged(int x, int y)
  {
    this.invokeCallback(this.cb_pointerDragged, x, y);
  }

  protected void pointerPressed(int x, int y)
  {
    this.invokeCallback(this.cb_pointerPressed, x, y);
  }

  protected void pointerReleased(int x, int y)
  {
    this.invokeCallback(this.cb_pointerReleased, x, y);
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
        this.showError(e, "Opening " + name + " failed " + e.getMessage());
      }

    return out;
  }

  private void showError(Throwable e, String s)
  {
    Alert msg = new Alert("Error", s, null, AlertType.INFO);
    msg.setTimeout(Alert.FOREVER);
    this.display.setCurrent(msg);
    e.printStackTrace();
    try {Thread.sleep(5000);} catch (Exception e2) {}
    main.notifyDestroyed();
  }

  /* The main thread function */
  public void run()
  {
    try {
      int start = CibylCallTable.getAddressByName("__start");
      int main = CibylCallTable.getAddressByName("main");
      int destructors = CibylCallTable.getAddressByName("crt0_run_global_destructors");

      /* Register some objects */
      Syscalls.canvasHandle = CRunTime.registerObject( this );
      Syscalls.graphicsHandle = CRunTime.registerObject( this.getGraphics() );
      Syscalls.midletHandle = CRunTime.registerObject( this.main );

      /* Start the virtual machine */
      int sp = (CRunTime.memory.length * 4) - 8;
      CibylCallTable.call(start, sp, 0, 0, 0, 0);
      CibylCallTable.call(main, sp, 0, 0, 0, 0);
      CibylCallTable.call(destructors, sp, 0, 0, 0, 0);

    } catch (OutOfMemoryError e) {
      this.showError(e, "Out of memory");
    } catch (Exception e) {
      this.showError(e, e.getMessage());
    }
  }
}
