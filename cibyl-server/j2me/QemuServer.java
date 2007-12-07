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
import javax.microedition.io.*;

class QemuServer
{
    private class Packet
    {
	public int type;
	public int value;
	public int args[];

	public static final int MSG_PEER_MEM_READ = 1;
	public static final int MSG_PEER_MEM_WRITE = 2;
	public static final int MSG_PEER_CALLBACK = 3;
	public static final int MSG_QEMU_REQ = 4;
	public static final int MSG_QEMU_MEM_VALUE = 5;
	public static final int MSG_OK = 6;

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
		System.out.println("bl�" + e);
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
		System.out.println("bl�" + e);
	    }
	}
    }

    private ServerSocketConnection connection;
    private DataInputStream is;
    private DataOutputStream os;

    public QemuServer()
    {
	this.connection = (ServerSocketConnection)Connector.open("socket://:9788");
    }

    private void error(String s) throws Exception
    {
	System.err.println(s);
	throw new Exception("Error");
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

    public void run()
    {
	SocketConnection socket;
	Packet p = new Packet();

	socket = (SocketConnection)this.connection.acceptAndOpen();

	this.is = socket.openDataInputStream();
	this.os = socket.openDataOutputStream();

	/* Read packets */
	while (true)
	    {
		p.read(is);
		switch (p.value)
		    {
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
