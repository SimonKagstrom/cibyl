 public static final int NOPH_Connector_openFileConnection(int __name) {
     String name = CRunTime.charPtrToString(__name);

     try {
         CRunTime.clearException();
         FileConnection ret = (FileConnection)Connector.open(name);
         int registeredHandle = CRunTime.registerObject(ret);
         return registeredHandle;
     }catch(Exception e) {
         CRunTime.setException(e);
         return 0;
     }
 }
