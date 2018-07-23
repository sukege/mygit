#include "work_que.h"

void factory_que_init(pque_t* pq,int capacity){
	pque_t pnew=(pque_t)calloc(1,sizeof(que_t));
	pnew->que_capacity=capacity;
	pthread_mutex_init(&pnew->que_mutex,NULL);
	*pq=pnew;
}

void factory_que_set(pque_t pq,pnode_t pnew){
	if(!pq->que_head){
		pq->que_head=pnew;
		pq->que_tail=pnew;
	}
	else{
		pq->que_tail->nd_next=pnew;
		pq->que_tail=pnew;
	}
	pq->que_size++;
}

void factory_que_get(pque_t pq,pnode_t* p){
	*p=pq->que_head;
	if(!*p){
		return;
	}
	(*p)->nd_next=NULL;
	pq->que_head=pq->que_head->nd_next;
	if(!pq->que_head){
		pq->que_tail=NULL;
	}
	pq->que_size--;
}

void factory_que_destroy(pque_t pq){
	pnode_t p=pq->que_head;
	pnode_t tmp;
	while(p){
		tmp=p;
		p=p->nd_next;
		free(tmp);
	}
	pq->que_head=NULL;
	pq->que_tail=NULL;
	pq->que_size=0;
}
