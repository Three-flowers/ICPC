/*************************************************************************
	> File Name: convolution_forward.c
	> Author: 
	> Mail: 
	> Created Time: 
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <athread.h>

#include "args.h"
#include "util.h"

extern void SLAVE_FUN(par_multihead_attn)(); //declare slave parallel method


int multihead_attention(Args_t arg)
{
	const int h_B = arg->B;
    const int h_S = arg->S;
    const int h_D = arg->D;
    const int h_N = arg->N;

    const float* h_x = arg->x;
    const float* h_w = arg->w;

    float* h_Q = arg->Q;
    float* h_K = arg->K;
    float* h_V = arg->V;
    float* h_QK = arg->QK;
    float* h_y = arg->y;

	const int h_PD = D/N;

    memset(h_Q, 0, sizeof(float)*B*S*D);
    memset(h_K, 0, sizeof(float)*B*S*D);
    memset(h_V, 0, sizeof(float)*B*S*D);
    memset(h_QK, 0, sizeof(float)*B*N*S*S);
    memset(h_y, 0, sizeof(float)*B*S*D);

	float* h_QN = (float*)aligned_malloc(sizeof(float)*B*N*S*PD, 128);
	float* h_KN = (float*)aligned_malloc(sizeof(float)*B*N*S*PD, 128);
	float* h_VN = (float*)aligned_malloc(sizeof(float)*B*N*S*PD, 128);

    athread_spawn(par_multihead_attn, arg); // spawn
    athread_join(); // wait for all slave threads finished
    return 0;
}

