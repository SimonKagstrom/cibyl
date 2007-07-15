	public static final void NOPH_RecordStore_setRecord(int _rs, int recordId, int _newData, int offset, int numBytes)
        {
	  RecordStore rs = (RecordStore)CRunTime.getRegisteredObject( _rs );
	  byte newData[] = new byte[numBytes];

	  for (int i = 0; i < numBytes; i++)
	    newData[i] = (byte)CRunTime.memoryReadByteUnsigned(_newData + offset + i);

	  try
	    {
	      CRunTime.clearException();
	      rs.setRecord(recordId, newData, 0, numBytes);
	    }
	  catch(Exception e)
	    {
	      CRunTime.setException(e);
	    }
	}
