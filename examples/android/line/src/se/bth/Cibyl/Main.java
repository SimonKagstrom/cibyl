/*********************************************************************
 *
 * Copyright (C) 2007,  Simon Kagstrom
 *
 * Filename:      Main.java
 * Author:        Simon Kagstrom <ska@bth.se>
 * Description:   Main class for Android programs for Cibyl
 *
 * $Id: StandaloneMain.java 13453 2007-02-05 16:28:37Z ska $
 *
 ********************************************************************/
package se.bth.Cibyl;

import java.io.*;

import android.content.*;
import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.Window;
import android.widget.TextView;
import android.util.Log;

/* Created from the LunarLander example */
public class Main extends Activity {
    CibylView view;

    @Override
    protected void onCreate(Bundle icicle) {
	super.onCreate(icicle);

	Log.v("Maboo", "---Main is here. Yo!");

        setContentView(R.layout.main);

	try {
	    Resources res = Resources.getSystem();
	    InputStream is = res.openRawResource(R.raw.program);

	    /* Setup the runtime support */
	    CRunTime.init(is);
	    is.close();
	    is = null;

	    Log.v("Maboo", "Starting the Cibyl program");
	    /* Start the virtual machine */
	    Cibyl.start(0, /* sp, set in crt0.S */
			0, /* a0 */
			0, /* a1 */
			0, /* a2 */
			0);/* a3 */
	} catch(Exception e) {
	    this.showError(e, "Opening program data failed " + e + ", " + e.getMessage());
	}
    }

    public void invokeCallback(int which, int a0, int a1, int a2, int a3)
    {
	try {
	    CRunTime.invokeCallback(which, a0, a1, a2, a3);
	} catch(Exception e) {
	    this.showError(e, "Calling " + Integer.toHexString(which) + " failed: " + e.getMessage());
	}
    }

    private void showError(Throwable e, String s)
    {
	Log.v("Maboo", "Error " + s);
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
	return false;
    }
}
