	public static final int NOPH_RecordStore_addRecord(int _rs, int _newData, int offset, int numBytes)
        {
	  RecordStore rs = (RecordStore)CRunTime.getRegisteredObject( _rs );
	  byte newData[] = new byte[numBytes];

	  for (int i = 0; i < numBytes; i++)
	    newData[i] = (byte)CRunTime.memoryReadByteUnsigned(_newData + offset + i);

	  try
	    {
	      CRunTime.clearException();
	      return rs.addRecord(newData, 0, numBytes);
	    }
	  catch(Exception e)
	    {
	      CRunTime.setException(e);
	    }

	  return 0;
	}
