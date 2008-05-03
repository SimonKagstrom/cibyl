/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      QemuServer.java
 * Author:        Simon Kagstrom <simon.kagstrom@gmail.com>
 * Description:   A server for Qemu stuff
 *
 * $Id:$
 *
 ********************************************************************/
import java.io.*;
import java.util.*;
import javax.microedition.io.*; /* Connector */

class QemuServer
{
    private class Packet
    {
	public int type;
	public int value;
	public int args[];

	public static final int MSG_PEER_MEM_READ   = 1;
	public static final int MSG_PEER_MEM_WRITE  = 2;
	public static final int MSG_PEER_CALLBACK   = 3;
	public static final int MSG_QEMU_REQ        = 4;
	public static final int MSG_QEMU_MEM_VALUE  = 5;
	public static final int MSG_QEMU_SET_STRTAB = 6;
	public static final int MSG_OK              = 7;

	public Packet()
	{
	    this.args = new int[16];
	}

	public void read(DataInputStream is)
	{
	    try {
		this.type = is.readInt();
		this.value = is.readInt();
		for (int i = 0; i < 16; i++)
		    {
			this.args[i] = is.readInt();
		    }
	    } catch (Exception e) {
		System.out.println("ble" + e);
	    }
	}

	public void write(DataOutputStream os)
	{
	    try {
		os.writeInt(this.type);
		os.writeInt(this.value);
		for (int i = 0; i < 16; i++)
		    {
			os.writeInt(this.args[i]);
		    }
	    } catch (Exception e) {
		System.out.println("ble" + e);
	    }
	}
    }

    private DataInputStream is;
    private DataOutputStream os;

    public QemuServer()
    {
    }

    private void error(String s)
    {
	System.err.println(s);
	//	throw new Exception("Error");
    }


    public int invokeCallback(int which, int a0, int a1, int a2, int a3)
    {
	Packet p = new Packet();

	p.type = Packet.MSG_PEER_CALLBACK;
	p.value = which;
	p.args[0] = a0;
	p.args[1] = a1;
	p.args[2] = a2;
	p.args[3] = a3;

	p.write(this.os);

	/* FIXME: This is actually wrong */
	p.read(this.is);
	if (p.type != Packet.MSG_OK)
	    this.error("Send MEM_WRITE but did not receive OK");

	return 0; // FIXME! This is wrong!
    }

    public void writeMem(int addr, int value)
    {
	Packet p = new Packet();

	p.type = Packet.MSG_PEER_MEM_WRITE;
	p.value = addr;
	p.args[0] = value;

	p.write(this.os);

	p.read(this.is);
	if (p.type != Packet.MSG_OK)
	    this.error("Send MEM_WRITE but did not receive OK");
    }

    public int readMem(int addr)
    {
	Packet p = new Packet();

	p.type = Packet.MSG_PEER_MEM_READ;
	p.value = addr;

	p.write(this.os);
	p.read(this.is);
	if (p.type != Packet.MSG_QEMU_MEM_VALUE)
	    this.error("Send MEM_READ but did not receive MEM_VALUE");
	return p.value;
    }

    public void parseStrTab(byte strs[])
    {
	int cur = 0;

	while (true)
	    {
		int first = cur;

		for (; strs[cur] != 0; cur++) /* Step through until NULL*/
		    ;

		Syscalls.addSyscall(first, new String(strs, first, cur - first));
		cur++;
		if (cur >= strs.length)
		    return;
	    }
    }

    private void setStrtab(Packet p)
    {
	int startAddress = p.args[0];
	int endAddress = p.args[1];
	byte strs[] = new byte[endAddress - startAddress];

	for (int addr = startAddress; addr < endAddress; addr += 4)
	    {
		int v = this.readMem(addr);

		strs[addr - startAddress + 0] = (byte)((v & 0xff));
		strs[addr - startAddress + 1] = (byte)((v & 0xff00) >> 8);
		strs[addr - startAddress + 2] = (byte)((v & 0xff0000) >> 16);
		strs[addr - startAddress + 3] = (byte)((v & 0xff000000) >> 24);
	    }
	this.parseStrTab(strs);
    }

    public void run()
    {
	Packet p = new Packet();

	try {
	    this.os = Connector.openDataOutputStream("file:///tmp/qemu-server-in");
	    this.is = Connector.openDataInputStream("file:///tmp/qemu-server-out");
	} catch(Exception e) {
	    System.err.println("Threw exception " + e);
	}

	/* Read packets */
	while (true)
	    {
		p.read(is);
		switch (p.value)
		    {
		    case Packet.MSG_QEMU_SET_STRTAB:
			this.setStrtab(p);
			break;
		    case Packet.MSG_QEMU_REQ:
			Syscalls.run(this, p.value, p.args);
			break;
		    default:
			/* Others are handled from Syscalls.run() */
			break;
		    }
	    }
    }
}
