public class SetjmpException extends Exception
{
    public SetjmpException(int cookie, int val)
    {
        this.cookie = cookie;
        this.val = val;
    }

    public int getCookie()
    {
        return this.cookie;
    }

    public int getValue()
    {
        return this.val;
    }

    private int cookie;
    private int val;
}
