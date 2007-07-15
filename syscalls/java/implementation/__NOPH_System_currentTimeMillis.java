public static void __NOPH_System_currentTimeMillis(int ptr)
{
    long time = System.currentTimeMillis();
    CRunTime.memoryWriteLong(ptr, time);
}
