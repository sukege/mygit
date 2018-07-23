#include <sys/uio.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <syslog.h>
#include <grp.h>
#include <time.h>
#include <pwd.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>                                 
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/mman.h>
#include <crypt.h>
#include <openssl/md5.h>
#include <mysql/mysql.h>
#include <sys/sendfile.h>

#define argu(a,b) {if(a!=b){printf("the argument's number is not right\n");return -1;}}
#define erro(a,b,c) {if(a==b){perror(c);}}
#define FILENAME "file"

extern ssize_t splice(int, off_t*,int,off_t*,size_t,unsigned int);
#define SPLICE_F_MORE 4
#define SPLICE_F_MOVE 1

typedef struct{
	char md5[128];
	int size;
	int sfd;
}updown,*pupdown;

typedef struct{
	pid_t pid;
	int fds;
}data;

typedef struct node{
	data d;
	struct node* pre;
	struct node* next;
}node,*pnode;

typedef struct{
	int len;
	char buf[1020];
}mess;

int tcp_init(char*,char*);
int tcp_connect(char*,char*);
pnode createnode(data);
void addnode(pnode*,pnode);
void delectnode(pnode*,pnode);
void createchild(pnode*,int);
void child_handle(int);
int send_n(int,char*,int);
int recv_n(int,char*,int);
void get_command(char*,char*,char*);
void* pthread_func_gets(void*);
void* pthread_func_puts(void*);
int get_md5(char*,char*);
void send_file(int,char*);
