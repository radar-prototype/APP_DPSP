#include <arm_neon.h>
#include "stdio.h"
#include <math.h>

float* modulo(float *a, float *b, const int len)
{
	float result[len];
	for(int i = 0; i<len; i++)
		result[i] = sqrt(a[i]*a[i] + b[i]*b[i]);
	return result;
}

float* modulo_neon(float *a, float *b, const int len)
{
	int dim4 = len >> 2;
	int left4 = len & 3;
	float result[len];
	float32x4_t result_vec;
	float32x4_t a_square;
	float32x4_t b_square;
	for(int i = 0; i<dim4; i++, a+=4, b+=4)
	{
		float32x4_t a_vec = vld1q_f32(a);
		float32x4_t b_vec = vld1q_f32(b);
		a_square = vmulq_f32(a_vec, a_vec);
		b_square = vmulq_f32(b_vec, b_vec);
		result_vec = vaddq_f32(a_square, b_square);
		for(int k = 0; k<4; k++)
		{
			result[i*4+k] = vgetq_lane_f32(result_vec, k);
		}
	}
	for(int i = 0; i<left4; i++, a++, b++)
		result[dim4*4+i] = sqrt((*a)*(*a) + (*b)*(*b));
	return result;
}