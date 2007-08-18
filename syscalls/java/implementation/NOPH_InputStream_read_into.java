/* From the optimized fread by Ehud Shabtai */
public static final int NOPH_InputStream_read_into(int obj, int ptr, int size, int eof_addr) throws Exception
{
  InputStream is = (InputStream)CRunTime.objectRepository[obj];
  int count = 0;

  byte[] buff = new byte[size];

  try {
    int r = is.read(buff);
    if (r < 0) throw new EOFException();
    CRunTime.memcpy(ptr, buff, 0, r);
    count += r;
  }
  catch(EOFException e) {
    CRunTime.memoryWriteShort( eof_addr, 1 );
  }
  buff = null;

  return count;
}
