import java.lang.*;
import java.io.*;

public class LibraryWrapper
{
  private int cb_add_to_arg, cb_get_int_pointer, cb_update_pointer;

  public LibraryWrapper()
  {
    try {
      this.cb_add_to_arg = CibylCallTable.getAddressByName("add_to_arg");
      this.cb_get_int_pointer = CibylCallTable.getAddressByName("get_int_pointer");
      this.cb_update_pointer = CibylCallTable.getAddressByName("update_pointer");
    } catch(Exception e)
      {
    	System.out.println(e.getMessage());
    	e.printStackTrace();
      }
  }

  public int add_to_arg(int v)
  {
    int sp = (CRunTime.memory.length * 4) - 8;
    try {
    return CibylCallTable.call(this.cb_add_to_arg, sp, v, 0, 0, 0);
    } catch(Exception e)
    {
    	System.out.println(e.getMessage());
    	e.printStackTrace();
    }
    return 0;
  }

  public int get_int_pointer()
  {
    int sp = (CRunTime.memory.length * 4) - 8;
    try {
    return CibylCallTable.call(this.cb_get_int_pointer, sp, 0, 0, 0, 0);
    } catch(Exception e)
    {
    	System.out.println(e.getMessage());
    	e.printStackTrace();
    }
    return 0;
  }

  public void update_pointer(int ptr)
  {
    int sp = (CRunTime.memory.length * 4) - 8;
    try {
    CibylCallTable.call(this.cb_update_pointer, sp, ptr, 0, 0, 0);
    } catch(Exception e)
    {
    	System.out.println(e.getMessage());
    	e.printStackTrace();
    }
  }
}
