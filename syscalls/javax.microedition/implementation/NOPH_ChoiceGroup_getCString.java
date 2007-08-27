	public static final void NOPH_ChoiceGroup_getCString(int __tf, int elementNum, int buffer, int size)
        {
		ChoiceGroup choice = (ChoiceGroup)CRunTime.objectRepository[__tf];
		String text = choice.getString(elementNum);

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
