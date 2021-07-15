#include <slave.h>
#include <math.h>
#include <simd.h>
#include <assert.h>
#include <string.h>

#include "args.h"

#define MAX_DATA 1024
#define J 64

extern const int h_B, h_S, h_D, h_N, h_PD;
extern const float *h_x, *h_w;
extern float *h_Q, *h_K, *h_V, *h_QK, *h_y, *h_QN, *h_KN, *h_VN;

// 调用并行模板
void par_multihead_attn(Args_t arg)
{
	volatile int get_reply, put_reply; 
	const int s_B, s_S, s_D, s_N, s_PD;
	const float s_x[MAX_DATA], s_w[MAX_DATA];
	float s_Q[MAX_DATA], s_K[MAX_DATA], s_V[MAX_DATA], s_QK[MAX_DATA], s_y[MAX_DATA], s_QN[MAX_DATA], s_KN[MAX_DATA], h_VN[MAX_DATA];

	par1(arg, get_reply, put_reply, 5);
	par2(arg, get_reply, put_reply, 6);

}


// 并行模板, 可根据情况删去部分参数
void par1(Args_t arg, volatile int get_reply, volatile int put_reply, int argn)  
{
	const int id = athread_get_id(-1);

	athread_get(PE_MODE, &h_x[0], &s_x[0], MAX_DATA * 4, &get_reply, 0, 0, 0);
	athread_get(PE_MODE, &h_w[0], &s_w[0], MAX_DATA * 4, &get_reply, 0, 0, 0);
	athread_get(PE_MODE, &h_Q[0], &s_Q[0], MAX_DATA * 4, &get_reply, 0, 0, 0);
	athread_get(PE_MODE, &h_K[0], &s_K[0], MAX_DATA * 4, &get_reply, 0, 0, 0);
	athread_get(PE_MODE, &h_V[0], &s_V[0], MAX_DATA * 4, &get_reply, 0, 0, 0);

	while (get_reply != argn);

	for (int b = 0; b < B; b++) {
		par_local_gemm_rcr(id, s_x[0]+b*S*D, D, s_w[0], D, s_Q[0]+b*S*D, D, S, D, D);
		par_local_gemm_rcr(id, s_x[0]+b*S*D, D, s_w[0]+D*D, D, s_K[0]+b*S*D, D, S, D, D);
        par_local_gemm_rcr(id, s_x[0]+b*S*D, D, s_w[0]+2*D*D, D, s_V[0]+b*S*D, D, S, D, D);
	}
	
	athread_put(PE_MODE, &s_x[0], &h_x[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_w[0], &h_w[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_Q[0], &h_Q[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_K[0], &h_K[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_V[0], &h_V[0], MAX_DATA * 4, &put_reply, 0, 0);
	
	while (put_reply != argn);
}

void par2(Args_t arg,volatile int get_reply, volatile int put_reply, int argn)  
{	

	const int id = athread_get_id(-1);


	athread_get(PE_MODE, &h_Q[0], &s_Q[0], MAX_DATA * 4, &get_reply, 0, 0, 0);
	athread_get(PE_MODE, &h_K[0], &s_K[0], MAX_DATA * 4, &get_reply, 0, 0, 0);
	athread_get(PE_MODE, &h_V[0], &s_V[0], MAX_DATA * 4, &get_reply, 0, 0, 0);
	athread_get(PE_MODE, &h_QN[0], &s_QN[0], MAX_DATA * 4, &get_reply, 0, 0, 0);
	athread_get(PE_MODE, &h_KN[0], &s_KN[0], MAX_DATA * 4, &get_reply, 0 ,0 ,0);
	athread_get(PE_MODE, &h_VN[0], &s_VN[0], MAX_DATA * 4, &get_reply, 0, 0, 0);

	while (get_reply != argn);

	par_local_trans_head(id, s_Q[0], s_QN[0], h_B, h_S, h_D, h_N);
    par_local_trans_head(id, s_K[0], s_KN[0], h_B, h_S, h_D, h_N);
    par_local_trans_head(id, s_V[0], s_VN[0], h_B, h_S, h_D, h_N);


	athread_put(PE_MODE, &s_Q[0], &h_Q[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_K[0], &h_K[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_V[0], &h_V[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_QN[0], &h_QN[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_KN[0], &h_KN[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_VN[0], &h_VN[0], MAX_DATA * 4, &put_reply, 0, 0);

	while (put_reply != argn);
}
static void par_local_gemm_rcr(const int id, const float* A, const int LDA, const float* B, const int LDB, float* C, const int LDC, int M, int N, int K) 
{
	for (; id < M; id += 64) {
		for(int j = 0; j < N; j ++)
            for(int k = 0; k < K; k ++)
                C[id*LDC+j] += A[id*LDA+k]*B[k+j*LDB];
	}
}

static void par_local_trans_head(const int id, float* src, float* dst, int B, inffft S, int D, int N)
{
    int pD = D/N;

#define SRC(b, s, d) src[b*S*D+s*D+d]
#define DST(b, n, s, pd) dst[b*N*S*pD + n*S*pD + s*pD + pd]

    for(; id < B; id += 64)
        for(int n = 0; n < N; n ++)
            for(int s = 0; s < S; s ++)
                for(int pd = 0; pd < pD; pd ++)
                    DST(b,n,s,pd) = SRC(b,s,n*pD+pd);
}

