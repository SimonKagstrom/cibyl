        private static final int FE_EVENT_NONE = 0;
        private static final int FE_EVENT_EXIT = 1;
        private static final int FE_EVENT_BACK = 2;
        private static final int FE_EVENT_SELECT = 4;
        private static final int FE_EVENT_LEFT = 8;
        private static final int FE_EVENT_RIGHT = 16;

	public static final int fe_get_buttons() {
	  GameCanvas gc = (GameCanvas)CRunTime.getRegisteredObject( Syscalls.canvasHandle);
	  int keys = gc.getKeyStates();
	  int out = Syscalls.FE_EVENT_NONE;

	  if ((keys & gc.LEFT_PRESSED) != 0)
	    out |= Syscalls.FE_EVENT_LEFT;
	  if ((keys & gc.RIGHT_PRESSED) != 0)
	    out |= Syscalls.FE_EVENT_RIGHT;
	  if ((keys & gc.DOWN_PRESSED) != 0)
	    out |= Syscalls.FE_EVENT_BACK;
	  if ((keys & gc.FIRE_PRESSED) != 0)
	    out |= Syscalls.FE_EVENT_SELECT;
	  if ((keys & gc.GAME_A_PRESSED) != 0)
	    out |= Syscalls.FE_EVENT_EXIT;

	  return out;
	}
