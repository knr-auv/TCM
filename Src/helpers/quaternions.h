#ifndef QUATERNIONS_H
#define QUATERNIONS_H
typedef struct{
    float r;
    float i;
    float j;
    float k;
}quaternion_t;

void Q_normalize(quaternion_t* Q);
void Q_fromEuler(float roll, float pitch, float yaw, quaternion_t* res);
void Q_multiply(quaternion_t* Q1, quaternion_t* Q2, quaternion_t* res);
float Q_norm(quaternion_t* Q);
void Q_conj(quaternion_t* Q);
quaternion_t Q_conj_new(quaternion_t* Q);
void Q_toEuler(quaternion_t* Q, float* res);
#endif