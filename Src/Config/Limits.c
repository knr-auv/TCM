#define DEG2RAD(x) x*3.1415f/180.f

float RollLimit = DEG2RAD(40);
float RollRateLimit = DEG2RAD(2);  

float PitchLimit = DEG2RAD(40);
float PitchRateLimit = DEG2RAD(2);

float YawRateLimit = DEG2RAD(2);



float LIMITS_GetRollLimit()
{
    return RollLimit;
}
float LIMITS_GetRollRateLimit()
{
    return RollRateLimit;
}

float LIMITS_GetPitchLimit()
{
    return PitchLimit;
}
float LIMITS_GetPitchRateLimit()
{
    return PitchRateLimit;
}

float LIMITS_GetYawRateLimit()
{
    return YawRateLimit;
}