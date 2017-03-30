#include "Lerp.h"

float Lerpf(float value, float in0, float in1, float out0, float out1)
{
    float normed = (value - in0) / (in1 - in0);
    float result = out0 + (normed * (out1 - out0));
    return result;
}