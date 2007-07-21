 public static final int NOPH_Connector_openFileConnection(int __name) throws Exception {
     String name = CRunTime.charPtrToString(__name);

     FileConnection ret = (FileConnection)Connector.open(name);
     int registeredHandle = CRunTime.registerObject(ret);
     return registeredHandle;
 }
