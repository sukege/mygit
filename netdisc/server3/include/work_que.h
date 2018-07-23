#ifndef __WORK_QUE_H__
#define __WORK_QUE_H__

#include "head.h"

typedef struct tag_node{
	int nd_sockfd;
	struct tag_node* nd_next;
}node_t,*pnode_t;

typedef struct tag_que{
	pnode_t que_head,que_tail;
	int que_capacity;
	int que_size;
	pthread_mutex_t que_mutex;
}que_t,*pque_t;

void factory_que_init(pque_t*,int);
void factory_que_set(pque_t pq,pnode_t);
void factory_que_get(pque_t pq,pnode_t* p);
void factory_que_destroy(pque_t pq); 

#endif
