	public static final void fe_init() {
	  int w = 240;
	  int h = 120;

	  GameCanvas gc = (GameCanvas)CRunTime.getRegisteredObject( Syscalls.canvasHandle ); /* See syscalls/j2me/init */
	  Graphics g = (Graphics)CRunTime.getRegisteredObject( Syscalls.graphicsHandle ); /* See syscalls/j2me/init */

	  g.setColor(156, 167, 143);
	  g.fillRect(0,0, w,h);
	  gc.flushGraphics();
	}
