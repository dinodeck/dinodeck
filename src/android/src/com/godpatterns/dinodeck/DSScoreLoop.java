package com.godpatterns.dinodeck;


import android.util.Log;
import java.util.List;
//
// SCORE LOOP CODE
//
import com.scoreloop.client.android.core.controller.RequestController;
import com.scoreloop.client.android.core.controller.RequestControllerObserver;
import com.scoreloop.client.android.core.controller.ScoreController;
import com.scoreloop.client.android.core.controller.ScoresController;
import com.scoreloop.client.android.core.model.Client;
import com.scoreloop.client.android.core.model.Score;
import com.scoreloop.client.android.core.model.Session;
import com.scoreloop.client.android.core.model.TermsOfService.Status;
import com.scoreloop.client.android.core.model.TermsOfService;
import com.scoreloop.client.android.core.controller.TermsOfServiceController;
import com.scoreloop.client.android.core.controller.TermsOfServiceControllerObserver;

class DDScoreLoop
{
    private static final String TAG = "ddscoreloop"; // used for logging
    private final String SECRET = "I0kGWMpRN2JieAfbX/7u8ZLCwkqq9XbYYU0nJhVBOx6ALQMtV5tP/Q==";
    private static DDScoreLoop mScoreLoop = null;
    private static DSActivity mDDActivity = null;
    private boolean mIsInitialized = false;

    public DDScoreLoop(DSActivity activity)
    {
        Log.v(TAG, "ddscoreloop oncreate called");
        mScoreLoop = this;
        mDSActivity = activity;
    }

    //
    // Calls from C++
    //
    public static void init(final String key)
    {
        mDDActivity.runOnUiThread
        (
            new Runnable()
            {
                public void run()
                {
                    Client.init(mScoreLoop.mDSActivity, key, null);
                    mScoreLoop.mIsInitialized = true;
                    Log.v(TAG, "ddscoreloop inited with key:" + key);
                }
            }
        );
    }

    public static String get_tos_state()
    {
        if(!mScoreLoop.mIsInitialized)
        {
            return "NOT_INITED";
        }
        Log.v(TAG, "Entered get_tos_state");
        TermsOfService tos = Session.getCurrentSession()
                                .getUsersTermsOfService();
        Log.v(TAG, tos.getStatus().toString());
        return tos.getStatus().toString();
    }

    public static boolean is_initialized()
    {
        return mScoreLoop.mIsInitialized;
    }

    public static void show_tos()
    {
        if(!mScoreLoop.mIsInitialized)
        {
            return;
        }

        mScoreLoop.mDSActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                TermsOfService tos = Session.getCurrentSession().getUsersTermsOfService();
                try {
                    // if (tos.getStatus() == Status.ACCEPTED) {
                    //     return;
                    // } else {
                        TermsOfServiceControllerObserver observer = new TermsOfServiceControllerObserver() {
                            @Override
                            public void termsOfServiceControllerDidFinish(
                                  TermsOfServiceController controller, Boolean accepted) {
                                try {
                                    if (accepted) {
                                        //activity.startActivity(new Intent(activity, EntryScreenActivity.class));
                                    }
                                } catch (NullPointerException ex) {
                                    //TODO: No Internet Connection, show error message
                                }
                            }
                        };
                        TermsOfServiceController controller = new TermsOfServiceController(observer);
                        controller.query(mScoreLoop.mDSActivity.getParent());
                   //}
                } catch (Exception e) {}
            }
        });
    }

    public static void push_score(final double primary, final double secondary, final int callbackId)
    {

        Log.v(TAG, "Entered get_tos_state");
        if(!mScoreLoop.mIsInitialized)
        {
            return;
        }


        mScoreLoop.mDDActivity.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                RequestControllerObserver scoreControllerObserver = new RequestControllerObserver()
                {
                    @Override
                    public void requestControllerDidReceiveResponse(RequestController controller)
                    {
                        // Handle Success
                        mScoreLoop.mDDActivity.nativeOnCallbackSuccess(callbackId, "");
                    }

                    @Override
                    public void requestControllerDidFail(RequestController controller, Exception exception)
                    {
                        // Handle Failure
                        mScoreLoop.mDDActivity.nativeOnCallbackFailure(callbackId, "");
                    }
                };

                final Score score = new Score(primary, null);
                // score.setMode(gameState.getMode()); // A int to signify the level

                ScoreController scoreController = new ScoreController(scoreControllerObserver);
                scoreController.submitScore(score);
            }
        });
    }

    public static void get_leaderboard(final int type, final int callbackId)
    {
        Log.v(TAG, "Entered get_leaderboard");
        if(!mScoreLoop.mIsInitialized)
        {
            return;
        }


        mScoreLoop.mDDActivity.runOnUiThread(new Runnable()
        {
            @Override
            public void run()
            {
                RequestControllerObserver scoresControllerObserver = new RequestControllerObserver()
                {
                    @Override
                    public void requestControllerDidReceiveResponse(RequestController controller)
                    {
                        // Handle Success
                        // Should be able to cast it
                        ScoresController myScoresController = (ScoresController) controller;
                        List<Score> retrievedScores = myScoresController.getScores();
                        StringBuilder builder = new StringBuilder("");

                        // Encode blob style
                        builder.append("[");
                        for(Score score : retrievedScores)
                        {
                            double mainScore = score.getResult();
                            int rank = score.getRank();
                            String displayName = score.getUser().getDisplayName();
                            builder.append("[");

                            // name -> value
                            builder.append("s4 name;");
                            builder.append(String.format("s%d %s",
                                           displayName.length(),
                                           displayName));

                            // score -> value
                            builder.append("s5 score;");
                            builder.append("n");
                            builder.append(mainScore);
                            builder.append("]");
                        }
                        builder.append("]");

                        mScoreLoop.mDSActivity.nativeOnCallbackSuccess(callbackId, builder.toString());
                    }

                    @Override
                    public void requestControllerDidFail(RequestController controller, Exception exception)
                    {
                        // Handle Failure
                        mScoreLoop.mDDActivity.nativeOnCallbackFailure(callbackId, "");
                    }
                };

                //Create an instance of the controller
                ScoresController myScoresController = new ScoresController(scoresControllerObserver);
                //myScoresController.setSearchList(SearchList.getBuddiesScoreSearchList());
                myScoresController.loadRangeForUser(Session.getCurrentSession().getUser());
            }
        });







        // //Set the searchlist. Default is globalScoreSearchList
        //

        // //Set the mode for the controller. Default mode
        // //myScoresController.setMode(2);

        // //Set the range length of scores to be retrieved. Default is 25
        // //myScoresController.setRangeLength(20);

        // //Make the request to the server
        // myScoresController.loadRangeForUser(Session.getCurrentSession().getUser());
    }
}