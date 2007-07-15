	public static final void NOPH_ChoiceGroup_getCString(int __tf, int elementNum, int buffer, int size)
        {
		System.out.println("In NOPH_ChoiceGroup_getCString");
		ChoiceGroup choice = (ChoiceGroup)CRunTime.objectRepository[__tf];
		String text = choice.getString(elementNum);
		System.out.println("In NOPH_ChoiceGroup_getCString, text:" + text + " size:" + size);

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
