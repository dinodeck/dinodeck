#ifndef SCORELOOP_H
#define SCORELOOP_H

#include "../reflect/Reflect.h"

class LuaState;

class ScoreLoop
{

    public: static Reflect Meta;
    public: static void Bind(LuaState* state);

    public:

        ScoreLoop(const char* secret);
        ~ScoreLoop();
        void PostScore(double primary, double secondary,
                       int onSuccessId, int onFailureId);
        std::string GetTOSState();
        void ShowTOS();
        bool IsInitialized();
        void GetLeaderboard(int type, int onSuccessId, int onFailureId);

};

#endif