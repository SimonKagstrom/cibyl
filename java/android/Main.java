/*********************************************************************
 *
 * Copyright (C) 2007,  Blekinge Institute of Technology
 *
 * Filename:      Main.java
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Main class for Android programs for Cibyl
 *
 * $Id: StandaloneMain.java 13453 2007-02-05 16:28:37Z ska $
 *
 ********************************************************************/
import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.Window;
import android.widget.TextView;

/* Created from the LunarLander example */
public class Main extends Activity {
    @Override
	protected void onCreate(Bundle icicle) {
	super.onCreate(icicle);
	try {
	    InputStream is = R.openRawResources(R.raw.program);

	    /* Setup the runtime support */
	    CRunTime.init(is);
	    is.close();
	    is = null;

	    /* Start the virtual machine */
	    Cibyl.start(0, /* sp, set in crt0.S */
			0, /* a0 */
			0, /* a1 */
			0, /* a2 */
			0);/* a3 */
	} catch(Exception e) {
	    this.showError(e, "Opening " + name + " failed " + e.getMessage());
	}
    }

    private DataInputStream getResourceStream(String name)
    {
	DataInputStream out = null;

	try {
	    InputStream stream = this.getClass().getResourceAsStream(name);
	    out = new DataInputStream(stream);
	} catch(Exception e) {
	    this.showError(e, "Opening " + name + " failed " + e.getMessage());
	}

	return out;
    }

    private void invokeCallback(int which, int a0, int a1, int a2, int a3)
    {
	try {
	    CRunTime.invokeCallback(which, a0, a1, a2, a3);
	} catch(Exception e) {
	    this.showError(e, "Calling " + Integer.toHexString(which) + " failed: " + e.getMessage());
	}
    }

    private void showError(Throwable e, String s)
    {
	System.out.println("Maboo: " + s);
    }

    @Override
	protected void onPause() {
	super.onPause();
    }

    @Override
	protected void onFreeze(Bundle outState) {
    }

    @Override
	public boolean onCreateOptionsMenu(Menu menu) {
	super.onCreateOptionsMenu(menu);
    }
}
