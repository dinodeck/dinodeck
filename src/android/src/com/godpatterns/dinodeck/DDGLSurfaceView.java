package com.godpatterns.dinodeck;

import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.Log;
import android.view.MotionEvent;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

class DDGLSurfaceView extends GLSurfaceView
{
    private static final String TAG = "ddglsurface_view"; // used for logging
    DDRenderer mRenderer;
    DDActivity mActivity;

    public DDGLSurfaceView(Context context, DDActivity thisActivity)
    {
        super(context);
        mActivity = thisActivity;
        mRenderer = new DDRenderer(context, thisActivity);
        setRenderer(mRenderer);
        // if (android.os.Build.VERSION.SDK_INT>=11)
        // {
        //     setPreserveEGLContextOnPause(true); //not possible on less than 3.0 (api 11)
        // }
    }

    public boolean onTouchEvent(final MotionEvent event)
    {
        final int action = event.getAction();
        switch (action)
        {
            case MotionEvent.ACTION_DOWN:
            {
                nativeOnTouch(1, event.getX(), event.getY());
                break;
            }
            case MotionEvent.ACTION_UP:
            {
                nativeOnTouch(2, event.getX(), event.getY());
                break;
            }
            case MotionEvent.ACTION_MOVE:
            {
                nativeOnTouch(3, event.getX(), event.getY());
                break;
            }
        }
        return true;
    }

    public void updateUI()
    {

    }

    public void gainedFocus()
    {
        super.onResume();
        if (mRenderer != null)
        {
            mRenderer.gainedFocus();
        }
    }

    public void onPause()
    {
        super.onPause();
    }

    private static native void nativeOnTouch(int touchEvent, float x, float y);
}
