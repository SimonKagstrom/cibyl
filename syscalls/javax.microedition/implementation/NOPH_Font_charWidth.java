	public static final int NOPH_Font_charWidth(int _fnt, int _ch)
	{
	  Font fnt = (Font)CRunTime.getRegisteredObject( _fnt );
	  char ch = (char)_ch;

	  return fnt.charWidth(ch);
	}
