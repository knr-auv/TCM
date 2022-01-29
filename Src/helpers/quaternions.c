

#include "arm_math.h"
#include "quaternions.h"
#include <math.h>
void Q_normalize(quaternion_t *Q)
{
    float norm = Q_norm(Q);
    Q->i /= norm;
    Q->j /= norm;
    Q->k /= norm;
    Q->r /= norm;
}
void Q_fromEuler(float roll, float pitch, float yaw, quaternion_t *res)
{
    roll *= M_PI / 180.f;
    pitch *= M_PI / 180.f;
    yaw *= M_PI / 180.f;
    float cy = arm_cos_f32(yaw * 0.5);
    float sy = arm_sin_f32(yaw * 0.5);
    float cp = arm_cos_f32(pitch * 0.5);
    float sp = arm_sin_f32(pitch * 0.5);
    float cr = arm_cos_f32(roll * 0.5);
    float sr = arm_sin_f32(roll * 0.5);

    res->r = cr * cp * cy + sr * sp * sy;
    res->i = sr * cp * cy - cr * sp * sy;
    res->j = cr * sp * cy + sr * cp * sy;
    res->k = cr * cp * sy - sr * sp * cy;
}
void Q_multiply(quaternion_t *Q1, quaternion_t *Q2, quaternion_t *res)
{
    res->r = (Q1->r * Q2->r - Q1->i * Q2->i - Q1->j * Q2->j - Q1->k * Q2->k);
    res->i = (Q1->r * Q2->i + Q2->r * Q1->i + Q1->j * Q2->k - Q1->k * Q2->j);
    res->j = (Q1->r * Q2->j - Q1->i * Q2->k + Q1->j * Q2->r + Q1->k * Q2->i);
    res->k = (Q1->r * Q2->k + Q1->i * Q2->j - Q1->j * Q2->i + Q1->k * Q2->r);
}
float Q_norm(quaternion_t *Q)
{
    float sum = Q->i * Q->i + Q->j * Q->j + Q->k * Q->k + Q->r * Q->r;
    arm_sqrt_f32(sum, &sum);
    return sum;
}
void Q_conj(quaternion_t *Q)
{
    Q->i = -Q->i;
    Q->j = -Q->j;
    Q->k = -Q->k;
}
quaternion_t Q_conj_new(quaternion_t* Q)
{
    quaternion_t r;
    r.r = Q->r;
    r.i = -Q->i;
    r.j = -Q->j;
    r.k = -Q->k;
    return r;
}

void Q_toEuler(quaternion_t *Q, float *res)
{
    Q_normalize(Q);
    float q_0 = Q->r;
    float q_1 = Q->i;
    float q_2 = Q->j;
    float q_3 = Q->k;

    res[2] = atan2f(2 * (q_1 * q_2 + q_0 * q_3), q_0 * q_0 + q_1 * q_1 - q_2 * q_2 - q_3 * q_3);
    res[1] = asinf(-2 * (q_1 * q_3 - q_0 * q_2));
    res[0] = atan2f(2 * (q_2 * q_3 + q_0 * q_1), q_0 * q_0 - q_1 * q_1 - q_2 * q_2 + q_3 * q_3);
    for(uint8_t i =0; i<3;i++)
        res[i] *=180.f/M_PI;
}
