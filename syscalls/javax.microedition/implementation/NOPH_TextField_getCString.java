	public static final void NOPH_TextField_getCString(int __tf, int buffer, int size)
        {
		TextField textField = (TextField)CRunTime.objectRepository[__tf];
		String text = textField.getString();

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
