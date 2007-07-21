	public static final int NOPH_RecordStore_getRecord(int _rs, int recordId, int _buffer, int offset) throws Exception
        {
	  RecordStore rs = (RecordStore)CRunTime.getRegisteredObject( _rs );

          byte tmp[] = rs.getRecord(recordId);
          for (int i = 0; i < tmp.length; i++)
              CRunTime.memoryWriteByte(_buffer + offset + i, tmp[i]);
          return tmp.length;
	}
