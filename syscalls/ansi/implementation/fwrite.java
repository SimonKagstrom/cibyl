public static final int fwrite(int ptr, int size, int nmemb, int fp)
{
    int items;

    for (items = 0; items < nmemb * size; items++)
	{
	    if (Syscalls.fputc(CRunTime.memoryReadByte(ptr + items), fp) == -1)
		return items;
	}

    return items;
}
