	public static final void fe_fill_area(int x, int y, int w, int h) {
	  GameCanvas gc = (GameCanvas)CRunTime.getRegisteredObject( Syscalls.canvasHandle ); /* See syscalls/j2me/init */
	  Graphics g = (Graphics)CRunTime.getRegisteredObject( Syscalls.graphicsHandle ); /* See syscalls/j2me/init */

	  g.setColor(0,0,0);
	  g.fillRect(x,y, w,h+1);
	  gc.flushGraphics(x,y,w,h+1);
	}
