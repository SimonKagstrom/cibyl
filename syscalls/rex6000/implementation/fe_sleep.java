	public static final void fe_sleep(int a0) {
	  int ms = a0 * 5;

	  try {
	    Thread.sleep(ms);
	  } catch (InterruptedException e) {}
	}
