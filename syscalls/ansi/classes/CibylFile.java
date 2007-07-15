import java.io.*;
#if defined(JSR075)
import javax.microedition.io.*;
import javax.microedition.io.file.*;
#endif /* JSR075 */

public class CibylFile
{
  public CibylFile(String path)
  {
    this.path = path;
#if defined(JSR075)
    this.fc = null;
#endif /* JSR075 */
    this.inputStream = null;
    this.outputStream = null;
    this.eof = 0;
    this.error = 0;
    this.offset = 0;
  }

  public int eof;
  public int error;
  public int offset;
  public String path;
#if defined(JSR075)
  public FileConnection fc;
#endif /* JSR075 */
  public InputStream inputStream;
  public OutputStream outputStream;
}
