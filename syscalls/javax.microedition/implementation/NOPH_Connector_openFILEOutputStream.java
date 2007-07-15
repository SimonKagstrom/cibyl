public static final int NOPH_Connector_openFILEOutputStream(int _name)
{
    String name = CRunTime.charPtrToString(_name);

    CRunTime.clearException();

    try
      {
	DataOutputStream s = Connector.openDataOutputStream(name);
	CibylFile f = new CibylFile(name);

	f.outputStream = (OutputStream)s;

	return CRunTime.registerObject(f);
      }
    catch (Exception e)
      {
	System.out.println("Could not open " + name + ", " + e);
	e.printStackTrace();
	CRunTime.setException(e);
	return 0;
      }
}
