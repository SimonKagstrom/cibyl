/* From the optimized fread by Ehud Shabtai */
public static final int NOPH_InputStream_read_into(int obj, int ptr, int size, int eof_addr) throws Exception
{
  InputStream is = (InputStream)CRunTime.objectRepository[__obj];
  int count = 0;

  byte[] buff = new byte[size];

  try {
    int r = is.read(buff);
    if (r < 0) throw new EOFException();
    if (r > 0) CRunTime.memcpy(ptr, buff, 0, r);
    buff = null;
  }
  catch(EOFException) {
    CRunTime.memoryWriteWord( addr_stdout, 1 );
  }

  return r;
}
