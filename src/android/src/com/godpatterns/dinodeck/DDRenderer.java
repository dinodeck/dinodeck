package com.godpattersn.dinodeck;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.DisplayMetrics;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

class DDRenderer implements GLSurfaceView.Renderer
{

    private static final String TAG = "ddrenderer"; // used for logging
    private Context mContext;
    private DDActivity mActivity;
    private int mWidth;
    private int mHeight;
    private boolean mResumeThisFrame;
    public static GL10 mGL;

    public DDRenderer(Context context, DDActivity thisActivity)
    {
        this.mContext = context;
        this.mActivity = thisActivity;
        this.mResumeThisFrame = false;
    }

    public void gainedFocus()
    {
        mResumeThisFrame = true;
    }

    public void onSurfaceCreated(GL10 gl, EGLConfig config)
    {
        mGL = gl;
        nativeClear();
    }

    public void onSurfaceChanged(GL10 gl, int w, int h)
    {
        mGL = gl;
        DisplayMetrics dm = new DisplayMetrics();
        mActivity.getWindowManager().getDefaultDisplay().getMetrics(dm);

        // will either be DENSITY_LOW, DENSITY_MEDIUM or DENSITY_HIGH
        int dpiClassification = dm.densityDpi;

        // these will return the actual dpi horizontally and vertically
        float xDpi = dm.xdpi;
        float yDpi = dm.ydpi;

        //once got size, can setup engine (in nativeResize)
        nativeResize(w, h, xDpi, yDpi);
    }

    public void onDrawFrame(GL10 gl)
    {
        if (mResumeThisFrame)
        {
            mResumeThisFrame = false;
            //nativeResume();
        }

        nativeUpdate(mActivity.deltaTime());

        mActivity.runOnUiThread(new Runnable() {
            public void run()
            {
                mActivity.update();
            }
        });
    }
    private native void nativeClear();
    private native void nativeUpdate(float dt);
    private native void nativeResize(int w, int h, float dpiX, float dpiY);
}