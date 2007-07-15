/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      Console.java
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Console
 *
 * $Id:$
 *
 ********************************************************************/
import javax.microedition.io.*;
import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;
import javax.microedition.lcdui.game.*;
import java.util.*;
import java.io.*;

class Console
{
    public Console()
    {
	/* Get the graphics object */
	int nr = Syscalls.NOPH_GameCanvas_get();
	this.canvas = (GameScreenCanvas)CRunTime.getRegisteredObject( nr );
	this.g = (Graphics)CRunTime.getRegisteredObject( Syscalls.NOPH_GameCanvas_getGraphics(nr) );
	this.buf = new String[10];

	for (int i = 0; i < this.buf.length; i++)
	    this.buf[i] = "";

	this.head = 0;
    }

    public void push(String s)
    {
	int textHeight = 12;
	int prevColor = this.g.getColor();
	Font prevFont = this.g.getFont();

	this.g.setFont( Font.getFont( Font.FACE_MONOSPACE,
				      Font.STYLE_PLAIN,
				      Font.SIZE_SMALL)  );


	this.g.setClip(0, this.canvas.getHeight() / 2,
		       this.canvas.getWidth(), this.canvas.getHeight() / 2);
	this.g.setColor(0,0,0);
	this.g.fillRect(0, this.canvas.getHeight() / 2,
			this.canvas.getWidth(), this.canvas.getHeight() / 2);
	this.g.setColor(0,255,0);

	this.buf[this.head] = s;
	this.head = (this.head + 1) % this.buf.length;

	for ( int i = 0; i < this.buf.length; i++ )
	    {
		int n = (i + this.head) % this.buf.length;

		this.g.drawString(buf[n], 0, this.canvas.getHeight() / 2 + i * textHeight + 2, Graphics.TOP|Graphics.LEFT);
	    }
	this.canvas.flushGraphics();

	this.g.setColor(prevColor);

	this.g.setClip(0, 0, this.canvas.getWidth(),
		       this.canvas.getHeight() / 2);
    }

    private GameScreenCanvas canvas;
    private Graphics g;
    private String buf[];
    private int head;
}
