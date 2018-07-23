#include "factory.h"

void factory_init(pfactory* pf,pthread_func_t pthread_func,int pthread_num,int capacity){
	pfactory pfnew=(pfactory)calloc(1,sizeof(factory));
	pfnew->pth_id=(pthread_t*)calloc(pthread_num,sizeof(pthread_t));
	factory_que_init(&pfnew->pq,capacity);
	pfnew->pthread_num=pthread_num;
	pfnew->pthread_func=pthread_func;
	pthread_cond_init(&pfnew->cond,NULL);
	*pf=pfnew;
}

void factory_start(pfactory f){
	if(!f->start_flag){
		for(int i=0;i<f->pthread_num;i++){
			pthread_create(f->pth_id+i,NULL,f->pthread_func,f);
		}
		f->start_flag=1;
	}
}
