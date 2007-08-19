import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;
import javax.microedition.lcdui.game.*;
import java.lang.*;
import java.util.*;
import java.io.*;

public class FormCommandMgr implements CommandListener,ItemStateListener
{
  private static int callback_addr;
  private static int callback_name;
  private static int callback_context;

  public static void setCallBackNotif(int callback_addr, int callback_name,
  					int callback_context) {

    FormCommandMgr.callback_addr = callback_addr;
    FormCommandMgr.callback_name = callback_name;
    FormCommandMgr.callback_context = callback_context;
  }

  private class CallBackContext {
    public int addr;
    public String name;
    public int context;

    public CallBackContext(int addr, String name, int context) {
    	this.addr = addr;
	this.name = name;
	this.context = context;
    }
  }

  private Hashtable commands = new Hashtable();
  private Form form;

  public FormCommandMgr(Form form) {
    this.form = form;
    form.setCommandListener(this);
    form.setItemStateListener(this);
  }

  public void addCommand(String name, int c_addr, String c_name, int c_context)
  {
    Command cmd = new Command(name, Command.SCREEN, 1);
    commands.put(cmd, new CallBackContext(c_addr, c_name, c_context));
    form.addCommand(cmd);
  }

  public void addCallback(Item item, int c_addr, String c_name, int c_context)
  {
    commands.put(item, new CallBackContext(c_addr, c_name, c_context));
  }

  private void checkAction(Object obj) {
    CallBackContext callback = (CallBackContext)commands.get(obj);
    if ((callback != null) && (callback_addr != 0)) {
      CRunTime.memoryWriteWord(callback_addr, callback.addr);
      int len = callback.name.getBytes().length;
      CRunTime.memcpy(callback_name, callback.name.getBytes(), 0, len);
      CRunTime.memoryWriteByte(callback_name+len, 0);
      CRunTime.memoryWriteWord(callback_context, callback.context);
    }
  }

  public void commandAction(Command cmd, Displayable d) {
    checkAction(cmd);
  }

  public void itemStateChanged(Item item)
  {
    System.out.println("item state changed for " + item);
    checkAction(item);
  }
}

