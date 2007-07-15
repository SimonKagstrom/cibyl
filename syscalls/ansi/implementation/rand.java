        public static final int rand()
        {
	  /* Remove the sign-bit */
	  return Syscalls.random.nextInt() & ~(1<<31);
        }
