	public static final void NOPH_TextField_getCString(int __tf, int buffer, int size)
        {
		System.out.println("In NOPH_TextField_getCString");
		TextField textField = (TextField)CRunTime.objectRepository[__tf];
		String text = textField.getString();
		System.out.println("In NOPH_TextField_getCString, text:" + text + " size:" + size);

		try {
			if (size > text.getBytes("UTF-8").length) size = text.getBytes("UTF-8").length;
			else size -= 1;

			System.out.print("str:");
			for (int i=0; i<size; i++) {
			  System.out.print(" "+(int)text.getBytes("UTF-8")[i] + ",");
			}
			System.out.println();
			CRunTime.memcpy(buffer, text.getBytes("UTF-8"), 0, size);
		} catch (UnsupportedEncodingException e) {
			CRunTime.memoryWriteByte(buffer+size, 0);
			return;
		}

		CRunTime.memoryWriteByte(buffer+size, 0);
	}
