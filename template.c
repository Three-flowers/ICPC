void par1(Args_t arg,volatile int get_reply, volatile int put_reply, int argn)  
{
	const int id = athread_get_id(-1);

	athread_get(PE_MODE, &h_x[0], &s_x[0], MAX_DATA * 4, &get_reply, 0, 0, 0);
	athread_get(PE_MODE, &h_w[0], &s_w[0], MAX_DATA * 4, &get_reply, 0, 0, 0);
	athread_get(PE_MODE, &h_Q[0], &s_Q[0], MAX_DATA * 4, &get_reply, 0, 0, 0);
	athread_get(PE_MODE, &h_K[0], &s_K[0], MAX_DATA * 4, &get_reply, 0, 0, 0);
	athread_get(PE_MODE, &h_V[0], &s_V[0], MAX_DATA * 4, &get_reply, 0, 0, 0);
	athread_get(PE_MODE, &h_QK[0], &s_QK[0], MAX_DATA * 4, &get_reply, 0 ,0 ,0);
	athread_get(PE_MODE, &h_y[0], &s_y[0], MAX_DATA * 4, &get_reply, 0, 0, 0);
	athread_get(PE_MODE, &h_QN[0], &s_QN[0], MAX_DATA * 4, &get_reply, 0, 0, 0);
	athread_get(PE_MODE, &h_KN[0], &s_KN[0], MAX_DATA * 4, &get_reply, 0 ,0 ,0);
	athread_get(PE_MODE, &h_VN[0], &s_VN[0], MAX_DATA * 4, &get_reply, 0, 0, 0);

	while (get_reply != argn);

	

	athread_put(PE_MODE, &s_x[0], &h_x[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_w[0], &h_w[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_Q[0], &h_Q[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_K[0], &h_K[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_V[0], &h_V[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_QK[0], &h_QK[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_y[0], &h_y[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_QN[0], &h_QN[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_KN[0], &h_KN[0], MAX_DATA * 4, &put_reply, 0, 0);
	athread_put(PE_MODE, &s_VN[0], &h_VN[0], MAX_DATA * 4, &put_reply, 0, 0);

	while (put_reply != argn);
}