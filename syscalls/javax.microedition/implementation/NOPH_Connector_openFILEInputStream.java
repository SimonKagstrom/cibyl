public static final int NOPH_Connector_openFILEInputStream(int _name)
{
    String name = CRunTime.charPtrToString(_name);

    CRunTime.clearException();

    try
      {
#if defined(JSR075)
	FileConnection fc = (FileConnection)Connector.open(name, Connector.READ);
	DataInputStream s = fc.openDataInputStream();
#else
	DataInputStream s = Connector.openDataInputStream(name);
#endif /* JSR075 */
	CibylFile f = new CibylFile(name);

#if defined(JSR075)
	f.fc = fc;
#endif /* JSR075 */
	f.inputStream = (InputStream)s;

	return CRunTime.registerObject(f);
      }
    catch (Exception e)
      {
	CRunTime.setException(e);
	return 0;
      }
}
