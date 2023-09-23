#pragma once

float COMMON_map(float x, float in_min, float in_max, float out_min, float out_max);
void COMMON_mat_vec_mul(float* matrix, float* vec, float* res, uint16_t rows, uint16_t columns);
void COMMON_linear_saturation(float* vector, uint16_t count, float max_value);