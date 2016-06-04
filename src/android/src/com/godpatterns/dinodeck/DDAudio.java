package com.godpatterns.dinodeck;


import android.app.Activity;
import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.SoundPool;
import android.util.Log;
import java.io.IOException;

public class DDAudio
{
    private static final String TAG = "ddaudio";
    private static DDActivity mActivity;

    public static MediaPlayer mMediaPlayer = null;
    public static String mCurrentStreamingFrom = "";
    public static boolean mStreamStarted = false;
    public static boolean mStreamPaused = false;
    public static SoundPool mSoundPool = null;
    private final static int MAX_SOUND_STREAMS = 8;

    public DDAudio(Context context, DDActivity activity)
    {
        mActivity = activity;
        mActivity.setVolumeControlStream(AudioManager.STREAM_MUSIC);

        // Third argument is sourceQuality
        // Which as of writing has no effect and 0 is the default.
        mSoundPool = new SoundPool(MAX_SOUND_STREAMS, AudioManager.STREAM_MUSIC, 0);
        mStreamStarted = false;
        mStreamPaused = false;
        mCurrentStreamingFrom = "";
    }

    public void onDestroy()
    {
        if(mMediaPlayer != null)
        {
            mMediaPlayer.pause();
            mMediaPlayer.stop();
            mMediaPlayer.release();
            mMediaPlayer = null;
        }
        mSoundPool.release();
        mSoundPool = null;
    }

    public static int load_sound(String path)
    {
        AssetManager am = mActivity.getAssets();
        Log.v(TAG, "load_sound [" + path + "]");
        try
        {
            AssetFileDescriptor fd = am.openFd(path);
            int soundId = mSoundPool.load(fd, 1);
            Log.v(TAG, "Loaded sound id: " + soundId);
            return soundId;
        }
        catch(IOException e)
        {
            Log.v(TAG, "failed to load [" + path + "]");
            return -1;
        }
    }

    public static void stop_sound(int soundId)
    {
        Log.v(TAG, "stop_sound [" + soundId + "]");
        mSoundPool.stop(soundId);
    }

    public static int play_sound(int soundId, boolean loop)
    {
        int loopValue = 0; // no loop
        if(loop)
        {
            loopValue = -1; // loop
        }
        float volume = 1.0f;
        Log.v(TAG, "playing sound " + soundId);
        int streamId = mSoundPool.play(soundId, volume, volume, 0, loopValue, 1.0f);
        if (streamId == 0)
        {
            Log.v(TAG, "failed to play sound");
        }
        else
        {
            Log.v(TAG, "Playing sound [" + streamId + "]");
        }
        return streamId;
    }

    public static void set_sound_volume(int id, float v)
    {
        mSoundPool.setVolume(id, v, v);
    }

    public static void sound_resume(int id)
    {
        mSoundPool.resume(id);
    }

    public static void sound_pause(int id)
    {
        mSoundPool.pause(id);
    }

    public static void stop_stream(int id)
    {
        if(mMediaPlayer == null)
        {
            return; // no media player no streaming.
        }

        Log.v(TAG, "Stop music");
        //set_stream_volume(0.0f);
        mMediaPlayer.pause();
        mMediaPlayer.stop();
    }

    public static void set_stream_volume(int id, float v)
    {
        if (mMediaPlayer == null)
        {
            return;
        }
        Log.v(TAG, "Setting stream volume");
        mMediaPlayer.setVolume(v, v);
    }

    public static void pause_stream(int id)
    {
        if(!mStreamStarted || mStreamPaused || mMediaPlayer == null)
        {
            Log.v(TAG, "pause_stream ignored because: "
                  + !mStreamStarted
                  + " " + mStreamPaused
                  + " " + (mMediaPlayer == null) + " ");
            return;
        }
        mMediaPlayer.pause();
        mStreamPaused = true;
    }

    public static void resume_stream(int id)
    {
        if(!mStreamStarted || !mStreamPaused || mMediaPlayer == null)
        {
            Log.v(TAG, "resume_stream ignored because: "
                  + mStreamStarted
                  + " " + mStreamPaused
                  + " " + (mMediaPlayer == null) + " ");
            return;
        }
        mMediaPlayer.start();
        mStreamPaused = false;
    }

    public static int play_stream(String path, boolean loop)
    {
        Log.v(TAG, "Play Stream [" + path + "]");
        AssetManager am = mActivity.getAssets();
        try
        {
            if(mStreamStarted && mCurrentStreamingFrom.equals(path))
            {
                resume_stream(1);
                return 1;
            }

            stop_stream(1); // only 1 stream.
            if(mMediaPlayer != null)
            {
                Log.v(TAG, "Mediaplayer already existed, releasing.");
                mMediaPlayer.release();
            }
            mMediaPlayer = new MediaPlayer();

            AssetFileDescriptor fd = am.openFd(path);
            mMediaPlayer.setDataSource
            (
                fd.getFileDescriptor(),
                fd.getStartOffset(),
                fd.getLength()
            );
            fd.close();

            mMediaPlayer.setLooping(loop);
            mMediaPlayer.setVolume(1, 1);
            mMediaPlayer.prepare();
            mMediaPlayer.start();
            mStreamStarted =true;
            mStreamPaused = false;
            mCurrentStreamingFrom = path;
            Log.v(TAG, "Told to play stream, returning.");
            return 1; // only 1 possible stream
        }
        catch(IOException e)
        {
            Log.v(TAG, "Failed to play stream ["+ path + "]");
            mCurrentStreamingFrom = "";
            return -1;
        }
    }
}