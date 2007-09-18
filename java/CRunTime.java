/*********************************************************************
 *
 * Copyright (C) 2006,  Blekinge Institute of Technology
 *
 * Filename:      CRunTime.java
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   C runtime support (compiled)
 *
 * $Id: CRunTime.java 14117 2007-03-10 09:32:26Z ska $
 *
 ********************************************************************/
import java.io.*;
import java.util.*;

public class CRunTime
{
  public static int memory[];

  public static int saved_v1; /* Result from functions */
  public static int ra; /* For the debug target */

  /* Pointer to the top of the event stack */
  public static int eventStackPointer;

  public static Object objectRepository[];
  private static int maxRepositoryObjects;
  private static int firstFree;

  /**
   * Initialize the C runtime. This must be called before the C
   * runtime is used.
   *
   * @param memoryImage the contents of the .data and .rodata sections
   * @param memorySize the total size of the memory, should be larger
   * than memoryImage to fit the .bss, the heap and the stack
   */
  public static final void init(int memoryImage[], int memorySize)
  {
    CRunTime.maxRepositoryObjects = 256;
    CRunTime.objectRepository = new Object[ CRunTime.maxRepositoryObjects ];
    CRunTime.objectRepository[0] = null;

    /* 0 is the invalid object, 1 is the exception object */
    CRunTime.firstFree = 2;

    CRunTime.memory = new int[memorySize / 4];

    /* Copy memory */
    for (int i=0; i<memoryImage.length; i++)
      CRunTime.memory[i] = memoryImage[i];

    CRunTime.memory[1] = CibylConfig.stackSize;
    CRunTime.memory[3] = memorySize - (CibylConfig.eventStackSize - 8);
  }

  public static final void init(DataInputStream codeStream)
  {
    CRunTime.memory = null;
    CRunTime.objectRepository = null;
    System.gc();

    int memorySize = (int)(Runtime.getRuntime().freeMemory() * CibylConfig.cibylMemoryProportion);

    if (CibylConfig.memorySize != 0)
      memorySize = (int)CRunTime.faultMemoryIn(CibylConfig.memorySize);

    /* See to it that the memory is aligned to 8. This caused a very
     * fun bug before in printf when called with "%f".
     *
     * Also setup the event stack at the very top of the memory
     */
    memorySize -= (memorySize & 7);
    CRunTime.eventStackPointer = memorySize - 8;

    try
      {
	int len = codeStream.available() / 4;
	int b[] = new int[ len ];
	int i = 0;

	if (len < 5)
	  {
	    /* This binary is broken - we need the header data */
	    System.out.println("ERROR: data input is too small");
	    return;
	  }

	while (i < len)
	  {
	    b[i] = codeStream.readInt();
	    i++;
	  }

	CRunTime.init(b, memorySize);
      }
    catch(Exception e)
      {
	System.err.println("Threw " + e);
	return;
      }
  }

  /**
   * TastePhone : Taste your MIDP Phone with this test MIDlet.<br>
   * <br>
   * Copyright (c) 2004, Thibaut REGNIER<br>
   * All rights reserved.<br>
   * <br>
   * Redistribution and use in source and binary forms, with or without modification,
   * are permitted provided that the following conditions are met:<br>
   * <br>
   *   * Redistributions of source code must retain the above copyright notice,
   *     this list of conditions and the following disclaimer.<br>
   *   * Redistributions in binary form must reproduce the above copyright notice,
   *     this list of conditions and the following disclaimer in the documentation and/or other materials
   *     provided with the distribution.<br>
   *   * Neither the name of the "Club des Utilisateurs de Java" nor the names of its contributors may be used to endorse
   *     or promote products derived from this software without specific prior written permission.<br>
   * <br>
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
   * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
   * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
   * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
   * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
   * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * @author Thibaut Regnier : mobilette@club-java.com
   */
  private static final long faultMemoryIn(int size)
  {
    long out = 0;
    Vector v= new Vector(1024, 256);
    int i = 0;

    try
      {
        // We run the garbage collector to be sure we have the maximum space available.
        System.gc();
        while (i < size / 1024)
          {
            int[] array= new int[256]; // int = 4 bytes => 256*4 = 1kB 
            v.addElement(array);
            i++;
            out = out + 1024;
          }
      }
    catch (Throwable t)
      { // Should get an OutOfMemoryError but we prefer to catch everything.
        v= null;
        out = Runtime.getRuntime().totalMemory();
        System.gc();
      }

    return out;
  }

  public static Object getRegisteredObject(int handle)
  {
    return CRunTime.objectRepository[handle];
  }

  public static int registerObject(Object obj)
  {
    int ret = CRunTime.firstFree;
    int i;

    // Invalid object
    if (obj == null)
        return 0;

    CRunTime.objectRepository[ret] = obj;
    for (i=CRunTime.firstFree; i < CRunTime.objectRepository.length; i++)
      {
	if (CRunTime.objectRepository[i] == null)
	  {
	    CRunTime.firstFree = i;
	    return ret;
	  }
      }

    if (i == CRunTime.objectRepository.length)
      {
	CRunTime.maxRepositoryObjects = CRunTime.maxRepositoryObjects * 2;
	Object tmp[] = new Object[CRunTime.maxRepositoryObjects];

	for (i = 0; i < CRunTime.objectRepository.length; i++)
	  tmp[i] = CRunTime.objectRepository[i];
	tmp[i] = obj;
	CRunTime.objectRepository = tmp;

	ret = i;
      }

    return ret;
  }

  public static Object deRegisterObject(int handle)
  {
    Object out = CRunTime.getRegisteredObject(handle);

    if (CRunTime.firstFree > handle)
      CRunTime.firstFree = handle;
    CRunTime.objectRepository[handle] = null;

    return out;
  }

  /* Misc. utils */
  public static String charPtrToString(int address)
  {
    int startAddress = address;
    int i = 0;
    int len = 0;

    while (CRunTime.memoryReadByte(startAddress + len) != 0)
      {
	len++;
      }

    if (len == 0)
      return "";

    byte vec[] = new byte[len];

    for (i = 0; i < len; i++)
      {
	vec[i] = (byte)CRunTime.memoryReadByte(startAddress + i);
      }
    try
      {
	String str;
	if (CibylConfig.stringEncoding == null)
	  str = new String(vec);
	else
	  str = new String(vec, CibylConfig.stringEncoding);

	return str;
      }
    catch (UnsupportedEncodingException e)
      {
	return "";
      }
  }

  public static final void memoryWriteByte(int address, int in)
  {
    int value = in & 0xff;
    int cur = CRunTime.memory[address / 4];
    int b = 3 - (address & 3);

    cur &= ~(0xff << b*8);

    cur |= (value << b*8);
    CRunTime.memory[address / 4] = cur;
  }

  public static final void memoryWriteShort(int address, int in)
  {
    int value = in & 0xffff;
    int cur = CRunTime.memory[address / 4];
    int b = 2 - (address & 2);

    cur &= ~(0xffff << b*8);

    cur |= (value << b*8);
    CRunTime.memory[address / 4] = cur;
  }

  public static final void memoryWriteWord(int address, int in)
  {
    CRunTime.memory[address / 4] = in;
  }

  public static final void memoryWriteLong(int address, long in)
  {
    CRunTime.memory[ address >> 2 ] = (int)(in >> 32);
    CRunTime.memory[ (address + 4) >> 2 ] = (int)(in & 0xffffffffl);
  }


  public static final long memoryReadLong(int address)
  {
    long low = ((long)CRunTime.memory[ (address + 4) >> 2 ]) & 0xffffffffl;
    long high  = ((long)CRunTime.memory[ address >> 2 ]) & 0xffffffffl;
    long out = (high << 32) | low;

    return out;
  }

  public static final int memoryReadWord(int address)
  {
    return CRunTime.memory[address / 4];
  }

  public static final int memoryReadByteUnsigned(int address)
  {
    int val = CRunTime.memory[address / 4];
    int b = 3 - (address & 3);

    return (val >>> (b*8)) & 0xff;
  }


  public static final int memoryReadByte(int address)
  {
    int out = CRunTime.memoryReadByteUnsigned(address);

    /* Sign-extend */
    if ( (out & (1<<7)) != 0 )
      return out | 0xffffff00;

    return out;
  }

  public static final int memoryReadShortUnsigned(int address)
  {
    int val = CRunTime.memory[address / 4];
    int b = 2 - (address & 2);

    return (val >>> (b*8)) & 0xffff;
  }

  public static final int memoryReadShort(int address)
  {
    int out = CRunTime.memoryReadShortUnsigned(address);

    /* Sign-extend */
    if ( (out & (1<<15)) != 0 )
      return out | 0xffff0000;

    return out;
  }

  public static final void memcpy(int addr, byte[] bytes, int off, int size)
  {
    while (((addr & 0x3) != 0) && (size > 0)) {
      byte b = bytes[off++];
      CRunTime.memoryWriteByte(addr, b);
      addr++;
      size--;
      if (size == 0) return;
    }

    while (size > 3) {
      int i = 0;
      for (int j=0; j<4; j++) {
	i = i << 8;
	int b = bytes[off++] & 0xff;
	i |= b;
      }

      CRunTime.memoryWriteWord(addr, i);
      addr += 4;
      size -= 4;
    }

    while (size > 0) {
      byte b = bytes[off++];
      CRunTime.memoryWriteByte(addr, b);
      addr++;
      size--;
    }
  }

  public static final void memcpy(byte[] bytes, int off, int addr, int size)
  {
    while (size > 0) {
      bytes[off++] = (byte)CRunTime.memoryReadByte(addr);
      addr++;
      size--;
    }

    return;
  }

  /* The nasty lwl/lwr and swl/swr instructions */
  public static final int memoryReadWordLeft(int address)
  {
    int b0 = CRunTime.memoryReadByteUnsigned(address + 3);
    int b1 = CRunTime.memoryReadByteUnsigned(address + 2);
    int b2 = CRunTime.memoryReadByteUnsigned(address + 1);
    int b3 = CRunTime.memoryReadByteUnsigned(address + 0);

    return (b0 | (b1 << 8) | (b2 << 16) | (b3 << 24));
  }

  public static final void memoryWriteWordLeft(int address, int rtVal)
  {
    CRunTime.memoryWriteByte(address + 3, (rtVal));
    CRunTime.memoryWriteByte(address + 2, (rtVal >> 8));
    CRunTime.memoryWriteByte(address + 1, (rtVal >> 16));
    CRunTime.memoryWriteByte(address + 0, (rtVal >> 24));
  }

  public static final void kill()
  {
    CRunTime.memory[-1] = 0;
  }

  public static final long divu(int rsVal, int rtVal)
  {
    long low, high;

    if (rsVal < 0 || rtVal < 0)
      {
	long a = rsVal;
	long b = rtVal;

	a &= 0xffffffffl;
	b &= 0xffffffffl;

        low = a / b;
        high = a % b;
      }
    else
      {
        low = rsVal / rtVal;
        high = rsVal % rtVal;
      }

    return (high << 32) | low;
  }

  public static final long multu(int rsVal, int rtVal)
  {
    long a = (long)rsVal;
    long b = (long)rtVal;

    a &= 0xffffffffl;
    b &= 0xffffffffl;

    return a * b;
  }

  public static final int sltu(int aVal, int bVal)
  {
    long a = (long)aVal;
    long b = (long)bVal;

    a &= 0xffffffffl;
    b &= 0xffffffffl;
    if (a < b)
      return 1;

    return 0;
  }

  private static int functionNesting;
  public static final void emitFunctionEnterTrace(String str)
  {
    for (int i = 0; i < CRunTime.functionNesting; i++)
      str = " " + str;
    CRunTime.functionNesting++;
    CRunTime.emitTrace(str);
  }

  public static final void emitFunctionExitTrace(String str)
  {
    CRunTime.functionNesting--;
  }

  // private static Console console;
  /* Used by cibyl-mips2java when tracing is turned on */
  public static final void emitTrace(String str)
  {
    // if ( console == null)
    //   CRunTime.console = new Console();
    // CRunTime.console.push(str);
    System.out.println(str);
  }
}
