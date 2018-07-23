#ifndef __FACTORY_H__
#define __FACTORY_H__
#include "head.h"
#include "work_que.h"
#define FILENAME "file"
typedef void*(*pthread_func_t)(void* p);
typedef struct{
	pthread_t *pth_id;
	int pthread_num;
	pque_t pq;
	pthread_func_t pthread_func;
	pthread_cond_t cond;
	short start_flag;
}factory,*pfactory;

typedef struct{
	char md5[128];
	off_t size;
	int sfd;
}updown,*pupdown;

typedef struct{
	int len;
	char buf[1020];
}mess;

void factory_init(pfactory*,pthread_func_t,int,int);
void factory_start(pfactory);
int tcp_init(char*,char*);
int tcp_connect(char*,char*);
void send_file(int);
int send_n(int,char*,int);
int recv_n(int,char*,int);
int query_users(int*,char*,char*,char*,int*);
int query_code(int,int*);
int query_procode(int*,int);
int query_md5(char*);
int query_md5id(char*,int);
int query_md5torm(char*,int);
int query_file(int,int,char*);
int query_path(int*,char*,int);
int query_filename(int,char (*)[1024],int);
int query_puts(char*,char*,off_t*,int);
int query_pwd(int,char*);
int insert_users(char*,char*,char*,int);
int insert_dir(int,int,char*);
int insert_file(int,int,char*,char*,long long);
int delete_code(int);
int changpath(int,int);
void get_command(char*,char*,char*);
void* pthread_func_gets(void*);
void* pthread_func_puts(void*);
void get_rand_str(char*,int);
void get_salt(char*);

#endif
