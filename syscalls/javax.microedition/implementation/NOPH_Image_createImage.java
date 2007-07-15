	public static final int NOPH_Image_createImage(int _src, int x, int y, int width, int height, int transform)
	{
	  Image src = (Image)CRunTime.getRegisteredObject( _src );
	  Image out = Image.createImage(src, x, y, width, height, transform);

	  return CRunTime.registerObject( out );
	}
