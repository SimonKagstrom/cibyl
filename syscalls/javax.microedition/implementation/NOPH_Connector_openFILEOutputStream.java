public static final int NOPH_Connector_openFILEOutputStream(int _name) throws Exception
{
    String name = CRunTime.charPtrToString(_name);

    DataOutputStream s = Connector.openDataOutputStream(name);
    CibylFile f = new CibylFile(name);

    f.outputStream = (OutputStream)s;

    return CRunTime.registerObject(f);
}
