	public static final void exit(int a0) {
#if defined(NOJ2ME)
	  System.exit(a0);
#else
	  MIDlet m = (MIDlet)CRunTime.getRegisteredObject(midletHandle);
	  m.notifyDestroyed();
#endif /* NOJ2ME */
	}
