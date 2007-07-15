	public static final void NOPH_Graphics_drawRGB(int __graphics, int rgbData, int offset, int scanlength,
						       int x, int y, int width, int height, int processAlpha)
        {
		Graphics graphics = (Graphics)CRunTime.objectRepository[__graphics];

		graphics.drawRGB(CRunTime.memory, (rgbData + offset) >> 2, scanlength, x, y, width, height, processAlpha != 0);
	}
