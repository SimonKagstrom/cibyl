        public static final void srand(int seed)
        {
	  Syscalls.random = new Random((long)seed);
        }
