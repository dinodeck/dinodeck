package com.godpatterns.dinodeck;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Handler;
import android.os.SystemClock;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;
import com.loopj.android.http.*;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.StringBuilder;
import java.lang.System;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;



// Dinodeck activity
// Android has a texture problem: http://code.google.com/p/android/issues/detail?id=12774
public class DDActivity extends Activity
{
    private static final String TAG = "ddactivity"; // used for logging
    private static DDGLSurfaceView mGLView = null;
    private static DDActivity mActivity = null;
    private static DDAudio mDDAudio = null;
    private static DDScoreLoop mDDScoreLoop = null;
    private Handler mUpdateTimeHandler = new Handler();
    private long mLastTimeCount;
    // // This should lock the screen to a given orientation.
    // // ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
    private static int mOrientation = ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
    private float mDeltaTime;


    // Http Post Variables
    private static AsyncHttpClient mClient = new AsyncHttpClient();
    private static String mUri = "";
    private static RequestParams mParams = null;
    private static String mOnSuccess = "";
    private static String mOnFailure = "";
    private static int mCallbackId = -1;


    private Runnable mUpdateTimeTask = new Runnable()
    {
        public void run()
        {
            if (mGLView != null)
            {
                mGLView.updateUI();
            }
            mUpdateTimeHandler.postDelayed(this, 500);

            update();
        }
    };
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        Log.v(TAG, "ddactivity oncreate called");
        // Make activity statically available for functions
        // talking to C
        mActivity = this;
        int dsRet = nativeOnCreate();

        // Alternatively I could have Java SetOrientate function.
        // This seems ok for now.
        if(dsRet == 0)
        {
            mOrientation = ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
            setRequestedOrientation(mOrientation);
        }
        else
        {
            mOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
            setRequestedOrientation(mOrientation);
        }

        super.onCreate(savedInstanceState);

        mClient.addHeader("Content-type", "application/x-www-form-urlencoded");
        mClient.addHeader("Accept", "text/plain");
        mGLView = null;



        mDDAudio = new DDAudio(this, this);
        mDDScoreLoop = new DDScoreLoop(this);
        mLastTimeCount = System.currentTimeMillis();
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        // Bundle extras = this.getIntent().getExtras();
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
        if(mDDAudio != null)
        {
            mDDAudio.onDestroy();
        }
    }

    void createGLView()
    {
        Log.v(TAG, "creating GL view");
        mGLView = new DDGLSurfaceView(this, this);
        mGLView.setZOrderOnTop(false);
        setContentView(mGLView);
    }

    @Override
    public void onConfigurationChanged(final Configuration newConfig)
    {
        // Ignore orientation change to keep activity from restarting
        super.onConfigurationChanged(newConfig);
    }

    @Override
    protected void onRestart()
    {
        super.onRestart();
        setRequestedOrientation(mOrientation);
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        setRequestedOrientation(mOrientation);
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
        super.onWindowFocusChanged(hasFocus);
        if(!hasFocus)
        {
            return;
        }

        setRequestedOrientation(mOrientation);

        if(mGLView == null)
        {
            Log.v(TAG, "Recreating GL view on resume");
            createGLView();
        }
        else
        {
            Log.v(TAG, "Resuming mGLView");
            mGLView.onResume();
        }

        if (mGLView != null)
        {
            mGLView.gainedFocus();
        }

        startTimer();
    }

    @Override
    public void onResume()
    {
        super.onResume();
        setRequestedOrientation(mOrientation);
    }

    public float deltaTime()
    {
        return mDeltaTime;
    }

    public void update()
    {
        long currentTimeCount = System.currentTimeMillis();
        mDeltaTime = (currentTimeCount - mLastTimeCount) / 1000.0f;
        mLastTimeCount = currentTimeCount;
    }

    public void startTimer()
    {
        mUpdateTimeHandler.removeCallbacks(mUpdateTimeTask);
        mUpdateTimeHandler.postDelayed(mUpdateTimeTask, 500);
    }

    public void stopTimer()
    {
        mUpdateTimeHandler.removeCallbacks(mUpdateTimeTask);
    }

    // Just here so it conforms to Dinodeck expectations
    // In production all assets should always exist!
    public static boolean does_asset_exist(String filename)
    {
        AssetManager am = mActivity.getAssets();
        InputStream is = null;
        boolean result = false;
        try
        {
            is = am.open(filename);
            is.close();
            result = true;
        }
        catch (IOException e)
        {
        }
        finally
        {
            try
            {
                if(is != null)
                {
                    is.close();
                }
            }
            catch (IOException e)
            {
            }
        }
        return result;
    }

    public static boolean open_asset(String filename)
    {
        Log.v(TAG, "open_asset called" + filename);
        AssetManager am = mActivity.getAssets();
        InputStream is = null;
        try
        {
            is = am.open(filename);
            int size = is.available();
            Log.v(TAG, "open_asset success " + filename + " "
                  + size + "bytes");
            byte[] buffer = new byte[size];
            is.read(buffer);
            is.close();
            nativeIFStream( buffer, size );
            buffer = null;
            return true;
        }
        catch (IOException e)
        {
            Log.v(TAG, "open_asset fail " + filename);
            return false;
        }
        finally
        {
            try
            {
                if(is != null)
                {
                    is.close();
                }
            }
            catch (IOException e)
            {
            }
        }

    }

    public static String read_save_data(String name)
    {
        Log.v(TAG, "read_save_data called with: [" + name + "]");

        File file = mActivity.getFileStreamPath(name);
        if(!file.exists())
        {
            return "";
        }

        FileInputStream fis = null;
        InputStreamReader isr = null;

        try
        {
            // I feel like there should be a - ReadAll or something
            // but I couldn't find it with a little searching.
            int nbytes = (int) file.length();
            char[] buffer = new char[nbytes];
            fis = mActivity.openFileInput(name);
            isr = new InputStreamReader(fis);
            isr.read(buffer);
            String data = new String(buffer);
            fis.close();
            isr.close();
            fis = null;
            isr = null;
            return data;
        }
        catch (Exception e)
        {
            Log.v(TAG, "read save data fail " + name);
            e.printStackTrace();
        }
        finally
        {
            try
            {
                if(fis != null)
                {
                    fis.close();
                }

                if(isr != null)
                {
                    isr.close();
                }
            }
            catch (IOException e)
            {
            }
        }
        return "";
    }

    public static void write_save_data(String name, String data)
    {
        Log.v(TAG, "write_save_data called with: [" + name + ", " + data +"]");

        FileOutputStream fos = null;

        try
        {
            fos = mActivity.openFileOutput(name, Context.MODE_PRIVATE);
            fos.write(data.getBytes());
            fos.close();
            fos = null;
        }
        catch (Exception e)
        {
            Log.v(TAG, "write save data fail " + name);
            e.printStackTrace();
        }
        finally
        {
            try
            {
                if(fos != null)
                {
                    fos.close();
                }
            }
            catch (IOException e)
            {
            }
        }
    }

    public static void start_http_post(String uri, int callbackId)
    {
        Log.v(TAG, "Start http post " + uri + " " + callbackId);
        mUri = uri;
        mParams = new RequestParams();
        mCallbackId = callbackId;
    }

    public static void finish_http_post()
    {
        Log.v(TAG, "finish http post data");

        // RequestParams params = new RequestParams();
        // params.put("data", "heresomsetest[];!\\u3000data");
        // Log.v(TAG, params.toString() );
        // Log.v(TAG, mParams.toString() );
        final int callbackId = mCallbackId;
        mClient.post(mUri, mParams,
        new AsyncHttpResponseHandler()
        {
            @Override
            public void onSuccess(String response)
            {
                Log.v(TAG, "onSuccess:" + response.toString());
                nativeOnCallbackSuccess(callbackId, response.toString());
                // call back into C create a copy of the mSuccess
            }

            @Override
            public void onStart()
            {
                Log.v(TAG, "// Initiated the request");
            }

            @Override
            public void onFailure(Throwable e, String response)
            {
                Log.v(TAG, "// Response failed :(" + response.toString());
                // call back into C create a copy of the mFailure
                nativeOnCallbackFailure(callbackId, response.toString());
            }

            @Override
            public void onFinish()
            {
                Log.v(TAG, "// Completed the request (either success or failure)");
                nativeOnCallbackFinish(callbackId);
            }
        });

        mParams = null;
        mOnSuccess = null;
        mOnFailure = null;
        mUri = null;
        mCallbackId = -1;
    }

    public static void add_http_post_data(String key, String value)
    {
        Log.v(TAG, "Add http post data " + key + ":" + value);
        mParams.put(key, value);
    }

    public static void exit()
    {
        mActivity.finish();
    }

    public static native void nativeIFStream( byte[] buffer, int iSize );
    public native int nativeOnCreate();

    public static native void nativeOnCallbackSuccess(int callbackId, String response);
    public static native void nativeOnCallbackFailure(int callbackId, String response);
    public static native void nativeOnCallbackFinish(int callbackId);

    static
    {
        System.loadLibrary("godpatterns");
    }
}


