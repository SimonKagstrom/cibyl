public static int NOPH_String_toCharPtr(int __str, int addr, int maxlen)
{
    String str = (String)CRunTime.objectRepository[__str];
    int i;

    for (i = 0; i < str.length(); i++)
        {
            if (i == maxlen-1)
                break;
            CRunTime.memoryWriteByte( addr+i, str.charAt(i) );
        }
    /* NULL-terminate */
    CRunTime.memoryWriteByte( addr+i, '\0' );

    return i;
}
