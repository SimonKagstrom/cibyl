import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;
import javax.microedition.lcdui.game.*;
import java.lang.*;
import java.util.*;
import java.io.*;

public class CommandMgr implements CommandListener
{
  private Hashtable commands = new Hashtable();
  private int addr;
  private static CommandMgr instance;

  private class MgrItem
  {
    public MgrItem(int callback, int arg)
    {
      this.callback = callback;
      this.arg = arg;
    }

    public int callback;
    public int arg;
  }

  private CommandMgr() {}

  public static CommandMgr getInstance()
  {
    if (instance == null) {
      instance = new CommandMgr();
    }

    return instance;
  }

  public void addCommand(int type, String name, int callback, int arg)
  {
    GameCanvas gc = (GameCanvas)CRunTime.getRegisteredObject(
                                        Syscalls.NOPH_GameCanvas_get());
    Command cmd = new Command(name, type, 1);
    commands.put(cmd, new MgrItem(callback, arg));
    gc.addCommand(cmd);
    if (commands.size() == 1) {
      gc.setCommandListener(this);
    }
  }

  public void commandAction(Command cmd, Displayable d) {
    MgrItem item = (MgrItem)commands.get(cmd);
    System.out.println("In commandAction: callback:" + item.callback + " arg:" + item.arg);
    if (item.callback != 0) {
        CompiledProgram.__CIBYL_global_jumptab(item.callback,
                                               CRunTime.eventStackPointer,
                                               item.arg, 0, 0, 0); /* a0 ... a3 */
    }
  }
}

