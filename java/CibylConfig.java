/**
 * Configuration for the Cibyl application. If you want to edit these
 * settings, copy it to your application directory, edit it and
 * install it into src/ when compiling your application.
 */
class CibylConfig
{
    /** Argument to GameCanvas constructor */
    public static final boolean supressKeyEvents = true;

    /** Memory size - 0 means use default size (Runtime.freeMemory() * CibylConfig.cibylMemoryProportion) */
    public static int memorySize = 0;

    /** The proportion of memory allocated to Cibyl (0..1) */
    public static float cibylMemoryProportion = 0.5f;

    /** If the memory should be "faulted" in. This can sometimes allow
     * for larger memory sizes, but can also sometimes lock the
     * application.
     */
    public static boolean faultMemoryIn = false;

    /** Stack size - 8KB by default */
    public static int stackSize = 8192;

    /** Event stack size - 4KB by default */
    public static int eventStackSize = 4096;

    /** The encoding of strings */
    public static String stringEncoding = null;

    /** Disable the addition of the Exit command */
    public static boolean disableExitCmd = false;
}
