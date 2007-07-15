/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      Main.java
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Main class for Cibyl
 *
 * $Id: Main.java 11608 2006-10-12 10:20:08Z ska $
 *
 ********************************************************************/
import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;

public class Main extends MIDlet implements CommandListener
{
  private GameScreenCanvas canvas;

  public void startApp()
  {
    if (canvas == null)
      {
	canvas = new GameScreenCanvas(Display.getDisplay(this), this);
	if (!CibylConfig.disableExitCmd)
	{
	  Command exitCommand = new Command("Exit", Command.EXIT, 0);
	  canvas.addCommand(exitCommand);
	  canvas.setCommandListener(this);
	}
      }

    /* Start the canvas */
    canvas.start();
  }

  public void pauseApp()
  {
  }

  public void destroyApp(boolean unconditional)
  {
  }

  public void commandAction(Command c, Displayable s)
  {
    if (c.getCommandType() == Command.EXIT)
      {
	destroyApp(true);
	notifyDestroyed();
      }
  }
}
