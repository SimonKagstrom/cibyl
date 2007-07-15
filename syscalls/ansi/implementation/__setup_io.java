public static final void __setup_io(int addr_stdin, int addr_stdout, int addr_stderr)
{
  CibylFile stdin = new CibylFile(null);
  CibylFile stdout = new CibylFile(null);
  CibylFile stderr = new CibylFile(null);

  CRunTime.memoryWriteWord( addr_stdin, CRunTime.registerObject(stdin) );
  CRunTime.memoryWriteWord( addr_stdout, CRunTime.registerObject(stdout) );
  CRunTime.memoryWriteWord( addr_stderr, CRunTime.registerObject(stderr) );

  //  stdin.inputStream = (InputStream)System.in;
  stdout.outputStream = (OutputStream)System.out;
  stderr.outputStream = (OutputStream)System.err;
}
