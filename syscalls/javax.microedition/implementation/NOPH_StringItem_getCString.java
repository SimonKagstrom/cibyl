	public static final void NOPH_StringItem_getCString(int __si, int buffer, int size)
        {
		StringItem stringField = (StringItem)CRunTime.objectRepository[__si];
		String text = stringField.getText();

		try {
			if (size > text.getBytes("UTF-8").length) size = text.getBytes("UTF-8").length;
			else size -= 1;

			CRunTime.memcpy(buffer, text.getBytes("UTF-8"), 0, size);
		} catch (UnsupportedEncodingException e) {
			CRunTime.memoryWriteByte(buffer+size, 0);
			return;
		}

		CRunTime.memoryWriteByte(buffer+size, 0);
	}
