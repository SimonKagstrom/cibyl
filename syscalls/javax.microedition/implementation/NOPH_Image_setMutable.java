	public static final int NOPH_Image_setMutable(int _src)
	{
	  Image image = (Image)CRunTime.getRegisteredObject( _src );
	  int w = image.getWidth();
	  int h = image.getHeight();
	  int[] rgbImage = new int[ w * h ];
	  image.getRGB( rgbImage, 0, w, 0, 0, w, h );
	  Image out = Image.createImage( w, h );
	  out.getGraphics().drawRGB( rgbImage, 0, w, 0, 0, w, h, true);
	  return CRunTime.registerObject( out );
	}
