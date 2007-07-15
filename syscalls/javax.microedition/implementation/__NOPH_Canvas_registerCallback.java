	public static final void __NOPH_Canvas_registerCallback(int which, int fnAddress) {
	    GameScreenCanvas canvas = (GameScreenCanvas)CRunTime.objectRepository[Syscalls.canvasHandle];

	    canvas.callbacks[which] = fnAddress;
	}
