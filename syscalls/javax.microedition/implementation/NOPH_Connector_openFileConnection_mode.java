 public static final int NOPH_Connector_openFileConnection_mode(int __name, int mode) throws Exception {
     String name = CRunTime.charPtrToString(__name);

     FileConnection ret = (FileConnection)Connector.open(name, mode);
     int registeredHandle = CRunTime.registerObject(ret);
     return registeredHandle;
 }
