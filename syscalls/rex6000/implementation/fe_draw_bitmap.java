        private static Hashtable bitmapHashTable;

        private static final Image fe_getImage(int address, int mode) {
	  if (Syscalls.bitmapHashTable == null)
	    Syscalls.bitmapHashTable = new Hashtable();

	  int modeAddress = mode != 0 ? (1<<30) : 0;
	  Image img = (Image)Syscalls.bitmapHashTable.get(new Integer(address + modeAddress));

	  /* Create a new instance of this image if it doesn't exist*/
	  if (img == null)
	    {
	      byte width = (byte)CRunTime.memoryReadByte(address);
	      byte height = (byte)CRunTime.memoryReadByte(address + 1);
	      int byte_w = (width/8) + ((width & 7) != 0 ? 1 : 0);
	      Graphics g;

	      img = Image.createImage(width, height + 1);
	      g = img.getGraphics();

	      for (int y = 0; y < height + 1; y++)
		{
		  for (int x = 0; x < width; x++)
		    {
		      byte curr = (byte)CRunTime.memoryReadByte(address + (y * byte_w + x/8) + 2);
		      int bit_on = ( curr & (0x80 >> (x & 7)) ) != 0 ? 1 : 0;

		      if ( (bit_on ^ mode) != 0 )
			g.setColor(0,0,0);
		      else
			g.setColor(156, 167, 143);
		      g.drawRect(x, y, 1, 1);
		    }
		}
	      Syscalls.bitmapHashTable.put(new Integer(address + modeAddress), img);
	    }
	  return img;
	}

	public static final void fe_draw_bitmap(int address, int x, int y, int mode) {
	  Image img = Syscalls.fe_getImage(address, mode);
	  GameCanvas gc = (GameCanvas)CRunTime.getRegisteredObject( Syscalls.canvasHandle ); /* See syscalls/j2me/init */
	  Graphics g = (Graphics)CRunTime.getRegisteredObject( Syscalls.graphicsHandle ); /* See syscalls/j2me/init */

	  g.drawImage(img, x, y, g.TOP | g.LEFT);
	  gc.flushGraphics(x,y, img.getWidth(), img.getHeight() );
	}
