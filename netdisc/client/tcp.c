#include "func.h"

int tcp_init(char* ip,char *port){
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	int reuse=1;
	setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
	struct sockaddr_in server;
	bzero(&server,sizeof(struct sockaddr));
	server.sin_family=AF_INET;
	server.sin_port=htons(atoi(port));
	server.sin_addr.s_addr=inet_addr(ip);
	bind(sfd,(struct sockaddr*)&server,sizeof(struct sockaddr));
	return sfd;
}

int tcp_connect(char* ip,char* port){
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	int reuse=1;
	setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
	struct sockaddr_in server;
	server.sin_family=AF_INET;
	server.sin_port=htons(atoi(port));
	server.sin_addr.s_addr=inet_addr(ip);
	connect(sfd,(struct sockaddr*)&server,sizeof(struct sockaddr));
	return sfd;
}
