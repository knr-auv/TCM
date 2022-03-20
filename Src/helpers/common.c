#include <stdint.h>
#include <math.h>
float COMMON_map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void COMMON_mat_vec_mul(float* matrix, float* vec, float* res, uint16_t rows, uint16_t columns)
{
  //matrix is row major
  for(uint16_t j= 0; j<rows; j++)
  {
    res[j] = 0;
    for(uint16_t i=0; i<columns;i++)
    {
      res[j] += vec[i] * matrix[i + j*columns];
    }
  }
}

void COMMON_linear_saturation(float* vector, uint16_t count, float max_value)
{
    float max =0;
     for(uint16_t i = 0;i<count;i++)
    {
        float temp = fabsf(vector[i]);
        if(temp>max)
            max = temp;
    }
    if(max>max_value)
    {
        float coef = max_value/max;
        for(uint16_t i = 0;i<count;i++)
        {
            vector[i]*=coef;
        }
    }
}