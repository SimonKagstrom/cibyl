/* Optimized fread by Ehud Shabtai */
public static final int fread(int ptr, int size, int nmemb, int fp)
{
  CibylFile f = (CibylFile)CRunTime.getRegisteredObject(fp);

  f.eof = 0;
  f.error = 0;

  size = size*nmemb;
  int count = 0;
  int chunk = size;
  if (chunk > 100000) chunk = 100000;
  byte[] buff = new byte[chunk];

  try{
    long t = System.currentTimeMillis();
    while (size > 0) {
      int r = f.inputStream.read(buff);
      if (r < 0) throw new IOException();
      if (r > 0) CRunTime.memcpy(ptr, buff, 0, r);
      f.offset += r;
      count += r;
      ptr += r;
      size -= r;
    }
    buff = null;

    return count;
  } catch(IOException e) {
    if (count > 0) {
      f.eof = 1;
      return count;
    }
    f.error = 1;
  }

  f.eof = 1;
  return 0; /* EOF */
}
