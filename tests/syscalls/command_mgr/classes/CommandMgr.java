



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
  private List list;
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

  public void setList(List l, int callback, int arg)
  {
    this.list = l;
    commands.put(List.SELECT_COMMAND, new MgrItem(callback, arg));
    l.setCommandListener(this);
  }

  public void addCommand(int type, String name, int callback, int arg)
  {
    int level = type == Command.EXIT ? 0 : 1;
    GameCanvas gc = (GameCanvas)CRunTime.getRegisteredObject(Syscalls.NOPH_Canvas_get());
    Command cmd = new Command(name, type, level);
    commands.put(cmd, new MgrItem(callback, arg));
    gc.addCommand(cmd);
    if (commands.size() == 1) {
      gc.setCommandListener(this);
    }
  }

  public void commandAction(Command cmd, Displayable d) {
    MgrItem item = (MgrItem)commands.get(cmd);

    if (item.callback != 0) {
      try {
        CibylCallTable.call(item.callback,
                            CRunTime.eventStackPointer,
                            item.arg, 0, 0, 0); /* a0 ... a3 */
        } catch(Exception e)
          {
            System.err.println(e);
          }
    }
    if (cmd == List.SELECT_COMMAND)
      {
        commands.remove(cmd);
        this.list = null;
        return;
      }

  }
}
