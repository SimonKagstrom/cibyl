public static final void __setup_io(int addr_stdout, int addr_stderr)
{
  CRunTime.memoryWriteWord( addr_stdout, CRunTime.registerObject(System.out) );
  CRunTime.memoryWriteWord( addr_stderr, CRunTime.registerObject(System.err) );
}
