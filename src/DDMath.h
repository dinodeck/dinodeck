
#ifndef DDMATH_H
#define DDMATH_H
#include <cmath>
//#define M_PI 3.14159265

static float DegreeToRadian(float angle)
{
   return M_PI * angle / 180.0;
}

static float RadianToDegree(float angle)
{
   return angle * (180.0 / M_PI);
}

#endif