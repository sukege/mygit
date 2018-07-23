#include "func.h"
#define N 5

void get_command(char* buf,char* com1,char* com2){
	int i=0,m=0,n=0;
	while(buf[i]==' '){i++;}
	while(buf[i]!=' '&&buf[i]&&i<127){
		com1[m++]=buf[i++];
	}
	while(buf[i]==' '){i++;}
	while(buf[i]!=' '&&buf[i]!='\n'&&buf[i]&&i<127){
		com2[n++]=buf[i++];
	}
}

typedef struct{
	char ip[48];
	char port[16];
	char name[1024];
	char passwd[1024];
	char command[1024];
	char filename[1024];
	off_t offset;
	off_t size;
	int fd;
}net,*pnet;

void* pthread_func_gets(void *p){
	pnet pn=(pnet)p;
	char tmp[2048]={0};
	off_t filesize=0;
	off_t seg=0;
	int sfd=tcp_connect(pn->ip,pn->port);
	send(sfd,pn->name,strlen(pn->name)-1,0);
	recv(sfd,tmp,sizeof(tmp),0);
	send(sfd,pn->passwd,strlen(pn->passwd),0);
	recv(sfd,tmp,sizeof(tmp),0);
	send(sfd,"ok",2,0);
	recv(sfd,tmp,sizeof(tmp),0);
	send(sfd,pn->command,strlen(pn->command),0);
	char *map;
 	if(recv(sfd,(char*)&filesize,sizeof(off_t),0)==0){
		printf("the file is not existed!!!\n");
		bzero(tmp,sizeof(tmp));
		sprintf(tmp,"%s%s","rm ",pn->filename);
		system(tmp);
		goto endgets;
	}
	seg=filesize/N;
	send(sfd,(char*)&pn->offset,sizeof(off_t),0);
	recv(sfd,tmp,sizeof(tmp),0);
	send(sfd,(char*)&pn->size,sizeof(off_t),0);
	lseek(pn->fd,0,SEEK_SET);
	lseek(pn->fd,filesize-1,SEEK_SET);
	write(pn->fd,"\0",1);
	if(pn->size>=pn->offset*seg){
		goto done;
	}
	map=(char*)mmap(NULL,filesize,PROT_READ|PROT_WRITE,MAP_SHARED,pn->fd,0);
	int download_size=0;
	int ret=0;
	if(pn->size>(pn->offset-1)*seg){
		download_size=pn->size;
		ret=0;
		printf("the file is downloading......\n");
		while(download_size<seg*pn->offset){
			ret=recv(sfd,map+download_size,seg*pn->offset-download_size,0);
			download_size+=ret;
			if(ret==0){
				printf("the download cancle!\n");
				goto endgets;
			}
		}
	}
	else{
		download_size=(pn->offset-1)*seg;
		ret=0;
		printf("the file is downloading......\n");
		while(download_size<seg*pn->offset){
			ret=recv(sfd,map+download_size,seg*pn->offset-download_size,0);
			download_size+=ret;
			if(ret==0){
				printf("the download cancle!\n");
				goto endgets;
			}
		}

	}
done:
	printf("the file have been download sucessfully!!!\n");
endgets:
	close(sfd);
	pthread_exit(NULL);
}

void* pthread_func_puts(void *p){
	pnet pn=(pnet)p;
	char tmp[1024]={0};
	int fd;
	char md5[128]={0};
	char filename[1024]={0};
	strcpy(filename,pn->filename);
	struct stat buf;
	int fds[2];
	pipe(fds);
	int sfd=tcp_connect(pn->ip,pn->port);
	send(sfd,pn->name,strlen(pn->name)-1,0);
	recv(sfd,tmp,sizeof(tmp),0);
	send(sfd,pn->passwd,strlen(pn->passwd),0);
	recv(sfd,tmp,sizeof(tmp),0);
	send(sfd,"ok",2,0);
	recv(sfd,tmp,sizeof(tmp),0);
	send(sfd,pn->command,strlen(pn->command),0);
	recv(sfd,tmp,sizeof(tmp),0);
	if(get_md5(filename,md5)==-1){
		goto endputs;
	}
	send(sfd,md5,strlen(md5),0);
	recv(sfd,tmp,sizeof(tmp),0);
	fd=open(filename,O_RDONLY);
	fstat(fd,&buf);
	send(sfd,(char*)&buf.st_size,sizeof(off_t),0);
	recv(sfd,tmp,sizeof(tmp),0);
	bzero(tmp,sizeof(tmp));
	send(sfd,"ok",2,0);
	recv(sfd,tmp,sizeof(tmp),0);
	if(strcmp(tmp,"exist")==0){
		printf("the file have already existed in the server.Congratulations!!!\n");
		goto endputs;
	}
	system("clear");
	printf("the file is uploading......\n");
	int ret=0;
	while(splice(fd,NULL,fds[1],NULL,32768,SPLICE_F_MORE|SPLICE_F_MOVE)>0)
	{
		ret=splice(fds[0],NULL,sfd,NULL,32768,SPLICE_F_MORE|SPLICE_F_MOVE);
		if(ret==-1){
			printf("the file upload fail!!!\n");
			goto endputs;
		}
	}
	printf("the file has uploaded sucess!!!\n");
endputs:
	close(sfd);
	pthread_exit(NULL);
}


int main(int argc,char** argv)
{
	pnet pn=(pnet)calloc(N,sizeof(net));
	system("clear");
	char com1[128]={0};
	char com2[128]={0};
	char buf[1024]={0};
	char salt[1024]={0};
	char password[1024]={0};
	char ip[128]="192.168.76.128";
	char port[N][48]={"2000","2001","2002","2003","2004"};
	char *passwd;
	int k;
	pthread_t thdgets[N]={0};
	pthread_t thdputs;
	int sfd=tcp_connect(ip,port[0]);
	for(k=0;k<N;k++){
		pn[k].offset=k+1;
		strcpy(pn[k].ip,ip);
		strcpy(pn[k].port,port[k]);
	}
	printf("please input your username:");
	fflush(stdout);
	read(0,buf,sizeof(buf));
	for(k=0;k<N;k++){
		strcpy(pn[k].name,buf);
	}
	send(sfd,buf,strlen(buf)-1,0);
	bzero(buf,sizeof(buf));
	recv(sfd,buf,sizeof(buf),0);
	sprintf(salt,"%s%s","$6$",buf);
retry:
	passwd=getpass("please input your password:");
	strcpy(password,crypt(passwd,salt));
	for(k=0;k<N;k++){
		strcpy(pn[k].passwd,password);
	}
	send(sfd,password,strlen(password),0);
	bzero(buf,sizeof(buf));
	recv(sfd,buf,sizeof(buf),0);
	if(strcmp(buf,"log in sucess")!=0){
		printf("wrong password please try again!!!\n");
		goto retry;
	}
	else{
		send(sfd,"ok",2,0);
		puts(buf);
	}
	int epfd=epoll_create(1);
	struct epoll_event event,evs[2];
	event.events=EPOLLIN;
	event.data.fd=sfd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&event);
	event.events=EPOLLIN;
	event.data.fd=0;
	epoll_ctl(epfd,EPOLL_CTL_ADD,0,&event);
	int ret1=0,ret2=0;
	while(1){
		ret1=epoll_wait(epfd,evs,2,-1);
		for(int i=0;i<ret1;i++){
			if(evs[0].data.fd==0){
				bzero(buf,sizeof(buf));
				bzero(com1,sizeof(com1));
				bzero(com2,sizeof(com2));
				for(k=0;k<N;k++){
					bzero(pn[k].command,sizeof(pn[k].command));
					bzero(pn[k].filename,sizeof(pn[k].filename));
				}
				read(0,buf,sizeof(buf));
				get_command(buf,com1,com2);
				if(!strcmp(com1,"puts")){
					system("clear");
					printf("connecting to the server...\n");
					memcpy(pn[0].command,buf,strlen(buf)-1);
					memcpy(pn[0].filename,com2,strlen(com2));
					send(sfd,"mark",4,0);
					pthread_create(&thdputs,NULL,pthread_func_puts,(void*)&pn[0]);
				}
				else if(!strcmp(com1,"gets")){
					system("clear");
					send(sfd,"mark",4,0);
					printf("connecting to the server...\n");
					struct stat buf1;
					int fd=open(com2,O_RDWR|O_CREAT,0600);
					fstat(fd,&buf1);
					for(k=0;k<N;k++){
						pn[k].fd=fd;
						pn[k].size=buf1.st_size;
						memcpy(pn[k].command,buf,strlen(buf)-1);
						memcpy(pn[k].filename,com2,strlen(com2));
						pthread_create(&thdgets[k],NULL,pthread_func_gets,(void*)&pn[k]);
					}
				}
				else{
					send(sfd,buf,strlen(buf)-1,0);
				}
			}
			if(evs[0].data.fd==sfd){
				bzero(buf,sizeof(buf));
				system("clear");
				ret2=recv(sfd,buf,sizeof(buf),0);
				if(ret2==0){
					printf("the server down!!!\n");
					return -1;
				}
				else{
					puts(buf);
				}
			}

		}
	}
	close(sfd);
}
