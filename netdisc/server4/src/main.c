#include "factory.h"
#define N 5

int fds[2];

void signal_func(int signum){
	char flag;
	write(fds[1],&flag,1);
}

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

void* pthread_func_gets(void *p){
	pupdown pud=(pupdown)p;
	int sfd=pud->sfd;
	off_t size=pud->size;
	char *map;
	int fd=open(pud->md5,O_RDWR|O_CREAT|O_TRUNC,0600);
	lseek(fd,size-1,SEEK_SET);
	write(fd,"\0",1);
	map=(char*)mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	int download_size=0;
	int ret=0;
	while(download_size<size){
		ret=recv(sfd,map+download_size,size,0);
		download_size+=ret;
		if(ret==0){
			printf("\nthe upload cancle suddenly!!!\n");
			goto endget;
		}
		printf("\ruploading %s%5.2f  ","%",100.0*download_size/size);
		fflush(stdout);
	}
	printf(" sucess!!!\n");
endget:
	pthread_exit(NULL);
}

void* pthread_func_puts(void *p){
	pupdown pud=(pupdown)p;
	off_t size=0;
	off_t offset=0;
	off_t seg=0;
	off_t filesize=pud->size;
	off_t fsize=pud->size;
	int sfd=pud->sfd;
	char md5[512]={0};
	seg=filesize/N;
	strcpy(md5,pud->md5);
	send(sfd,(char*)&filesize,sizeof(off_t),0);
	recv(sfd,(char*)&offset,sizeof(off_t),0);
	send(sfd,"ok",2,0);
	recv(sfd,(char*)&size,sizeof(off_t),0);
	if(size>=seg*offset){
		goto endput;
	}
	int ret=0;
	off_t load=0;
	int fd=open(md5,O_RDWR|O_CREAT,0600);
	if(size>seg*(offset-1)){
		lseek(fd,size,SEEK_SET);
		filesize=seg*offset;
		ret=0;
		load=size;
		if(offset!=N){
			while(load<seg*offset)
			{
				ret=sendfile(sfd,fd,&load,filesize-ret);
				load+=ret;
				if(ret==-1){
					goto endput;
				}
			}
		}
		else{
			while(load<fsize)
			{
				ret=sendfile(sfd,fd,&load,fsize-ret);
				load+=ret;
				if(ret==-1){
					goto endput;
				}

			}
		}
	}
	else{
		lseek(fd,seg*(offset-1),SEEK_SET);
		filesize=seg*offset;
		ret=0;
		load=seg*(offset-1);
		if(offset!=N){
			while(offset<filesize)
			{
				ret=sendfile(sfd,fd,&load,filesize-ret);
				load+=ret;
				if(ret==-1){
					goto endput;
				}
			}
		}
		else{
			while(offset<fsize)
			{
				ret=sendfile(sfd,fd,&load,fsize-ret);
				load+=ret;
				if(ret==-1){
					goto endput;
				}
			}
		}
	}
endput:
	printf("the file have been downloaded sucess!!!\n");
	pthread_exit(NULL);
}

void* pthread_func(void *p){
	pfactory pf=(pfactory)p;
	pnode_t pcur;
	updown ud;
	time_t start,end;
	int command=0;
	int ret=0;
	char com1[128]={0};
	char com2[128]={0};
	char buf[1024]={0};
	char name[1024]={0};
	char salt[1024]={0};
	char password[1024]={0};
	char filename[128][1024]={0};
	int path=0;
	int pathtmp=0;
	int code=0;
	int userid=0;
	int rete=0;
	int retu=0;
	pthread_t thdgets,thdputs;
	int i=0;
	while(1){
		pthread_mutex_lock(&pf->pq->que_mutex);
		if(!pf->pq->que_size){
			pthread_cond_wait(&pf->cond,&pf->pq->que_mutex);
		}
		factory_que_get(pf->pq,&pcur);
		pthread_mutex_unlock(&pf->pq->que_mutex);
		if(pcur!=NULL&&pcur->nd_sockfd!=-1){
			ud.sfd=pcur->nd_sockfd;
			bzero(buf,sizeof(buf));
			recv(pcur->nd_sockfd,name,sizeof(name),0);
			int re=query_users(&userid,name,salt,password,&path);
			printf("the user [%s] is connecting\n",name);
			if(re==0){
				send(pcur->nd_sockfd,salt,sizeof(salt),0);
retry:
				bzero(buf,sizeof(buf));
				recv(pcur->nd_sockfd,buf,sizeof(buf),0);
				if(strcmp(buf,password)==0){
					strcpy(buf,"log in sucess");
					send(pcur->nd_sockfd,buf,strlen(buf),0);
					recv(pcur->nd_sockfd,buf,sizeof(buf),0);
					bzero(buf,sizeof(buf));
					query_file(path,userid,buf);
					send(pcur->nd_sockfd,buf,strlen(buf)-1,0);
					start=time(NULL);
					end=time(NULL);
				}
				else{
					strcpy(buf,"wrong");
					send(pcur->nd_sockfd,buf,sizeof(buf),0);
					goto retry;
				}
			}
			else{
				get_salt(salt);
				send(pcur->nd_sockfd,salt,sizeof(salt),0);
				bzero(password,sizeof(password));
				recv(pcur->nd_sockfd,password,sizeof(password),0);
				path=0;
				insert_users(name,salt,password,path);
				query_users(&userid,name,salt,password,&path);
				bzero(buf,sizeof(buf));
				strcpy(buf,"log in sucess");
				send(pcur->nd_sockfd,buf,strlen(buf),0);
				recv(pcur->nd_sockfd,buf,sizeof(buf),0);
				bzero(buf,sizeof(buf));
				query_file(path,userid,buf);
				send(pcur->nd_sockfd,buf,strlen(buf)-1,0);
				start=time(NULL);
				end=time(NULL);
			}
			int epfd=epoll_create(1);
			struct epoll_event event,evs[1];
			event.events=EPOLLIN;
			event.data.fd=pcur->nd_sockfd;
			epoll_ctl(epfd,EPOLL_CTL_ADD,pcur->nd_sockfd,&event);
			while(1){
				if((end-start)>=60){
					printf("\nTime is up,client [%s] quit!!!\n",name);
					close(pcur->nd_sockfd);
					break;
				}
				rete=epoll_wait(epfd,evs,1,1000);
				end=time(NULL);
				printf("\rthe connected time is %02lds",end-start);
				fflush(stdout);
				if(rete>0&&evs[0].data.fd==pcur->nd_sockfd){
					start=time(NULL);
					end=time(NULL);
					bzero(buf,sizeof(buf));
					ret=recv(pcur->nd_sockfd,buf,sizeof(buf),0);
					if(ret==0){
						printf("the client [%s] has downed\n",name);
						break;
					}
					bzero(com1,sizeof(com1));
					bzero(com2,sizeof(com2));
					get_command(buf,com1,com2);
					if(!strcmp(com1,"cd")){command=1;}
					else if(!strcmp(com1,"pwd")){command=2;}
					else if(!strcmp(com1,"ls")||!strcmp(com1,"ll")||!strcmp(com1,"l")){
						command=3;
					}
					else if(!strcmp(com1,"rm")||!strcmp(com1,"remove")){command=4;}
					else if(!strcmp(com1,"mkdir")){command=5;}
					else if(!strcmp(com1,"puts")){command=6;}
					else if(!strcmp(com1,"gets")){command=7;}
					else if(!strcmp(com1,"mark")){command=8;}
					else{command=9;}
					switch(command){
						case 1:
							if(!strcmp(com2,"..")){
								bzero(buf,sizeof(buf));
								if(path==0){
									query_file(path,userid,buf);
									send(pcur->nd_sockfd,buf,strlen(buf)-1,0);
								}
								else{
									query_procode(&path,path);
									query_file(path,userid,buf);
									send(pcur->nd_sockfd,buf,strlen(buf)-1,0);
								}
							}
							else if(!strcmp(com2,".")){
								bzero(buf,sizeof(buf));
								query_file(path,userid,buf);
								send(pcur->nd_sockfd,buf,strlen(buf)-1,0);
							}
							else{
								i=0;
								memset(filename,0,sizeof(filename));
								query_filename(path,filename,userid);
								while(filename[i]!=NULL&&i<127){
									if(!strcmp(filename[i],com2)){
										query_path(&pathtmp,filename[i],userid);
										break;
									}
									i++;
								}
								if(i==127){
									bzero(buf,sizeof(buf));
									strcpy(buf,"the file or directory not exist!!!");
									send(pcur->nd_sockfd,buf,sizeof(buf),0);
								}
								else if(query_md5torm(buf,pathtmp)==0){
									bzero(buf,sizeof(buf));
									strcpy(buf,"it is not a directory!!!");
									send(pcur->nd_sockfd,buf,sizeof(buf),0);
								}
								else{
									path=pathtmp;
									bzero(buf,sizeof(buf));
									query_file(path,userid,buf);
									send(pcur->nd_sockfd,buf,strlen(buf)-1,0);
								}
							}
							break;
						case 2:
							bzero(buf,sizeof(buf));
							query_pwd(path,buf);
							send(pcur->nd_sockfd,buf,sizeof(buf),0);
							break;
						case 3:
							bzero(buf,sizeof(buf));
							query_file(path,userid,buf);
							send(pcur->nd_sockfd,buf,strlen(buf)-1,0);
							break;
						case 4:
							i=0;
							bzero(buf,sizeof(buf));
							bzero(filename,sizeof(filename));
							query_filename(path,filename,userid);
							while(filename[i]&&i<127){
								if(!strcmp(filename[i],com2)){
									query_path(&code,filename[i],userid);
									break;
								}
								i++;
							}
							if(i==127){
								strcpy(buf,"the file or directory not exist!!!");
								send(pcur->nd_sockfd,buf,sizeof(buf),0);
							}
							else{
								delete_code(code);
								strcpy(buf,"the file or directory have been removed!!!");
								send(pcur->nd_sockfd,buf,sizeof(buf),0);
							}
							break;
						case 5:
							insert_dir(path,userid,com2);
							bzero(buf,sizeof(buf));
							query_file(path,userid,buf);
							send(pcur->nd_sockfd,buf,strlen(buf)-1,0);
							break;
						case 6:
							send(pcur->nd_sockfd,"hello1",6,0);
							if(recv(pcur->nd_sockfd,ud.md5,sizeof(ud.md5),0)==0){
								close(pcur->nd_sockfd);
								pthread_exit(NULL);
							}
							send(pcur->nd_sockfd,"hello2",6,0);
							recv(pcur->nd_sockfd,&(ud.size),sizeof(off_t),0);
							send(pcur->nd_sockfd,"hello3",6,0);
							if(query_md5(ud.md5)==-1){
								recv(pcur->nd_sockfd,buf,sizeof(buf),0);
								send(pcur->nd_sockfd,"null",4,0);
								insert_file(path,userid,com2,ud.md5,ud.size);
								pthread_create(&thdgets,NULL,pthread_func_gets,(void*)&ud);
								pthread_join(thdgets,NULL);
								bzero(buf,sizeof(buf));
								close(pcur->nd_sockfd);
								pthread_exit(NULL);
							}
							else{
								retu=query_md5id(ud.md5,userid);
								if(retu==-1){
									insert_file(path,userid,com2,ud.md5,ud.size);
								}
								recv(pcur->nd_sockfd,buf,sizeof(buf),0);
								send(pcur->nd_sockfd,"exist",5,0);
								bzero(buf,sizeof(buf));
								close(pcur->nd_sockfd);
								pthread_exit(NULL);
							}
							break;
						case 7:
							retu=query_puts(com2,ud.md5,&(ud.size),userid);
							printf("the number is %d\n",retu);
							if(retu==0){
								pthread_create(&thdputs,NULL,pthread_func_puts,(void*)&ud);
								pthread_join(thdputs,NULL);
								close(pcur->nd_sockfd);
								pthread_exit(NULL);
							}
							else{
								close(pcur->nd_sockfd);
								pthread_exit(NULL);
							}
							break;
						case 8:
							break;
						default :
							bzero(buf,sizeof(buf));
							strcpy(buf,"Wrong command format!!!\nplease input again!!!");
							send(pcur->nd_sockfd,buf,sizeof(buf),0);
							break;
					}
					changpath(userid,path);
				}
			}
			free(pcur);
		}
		else if(pcur!=NULL&&pcur->nd_sockfd==-1){
			pthread_mutex_lock(&pf->pq->que_mutex);
			factory_que_set(pf->pq,pcur);
			pthread_mutex_unlock(&pf->pq->que_mutex);
			pthread_cond_broadcast(&pf->cond);
			printf("the pthread exit!!!\n");
			pthread_exit(NULL);
		}
	}
}

int main(int argc,char** argv){
	pipe(fds);
	argu(argc,2);
	int config=open(argv[1],O_RDONLY);
	char cfg[128]={0};
	read(config,cfg,sizeof(cfg));
	char ip[128]={0};
	char port[128]={0};
	get_command(cfg,ip,port);
	close(config);
	system("clear");
	if(fork()){
		close(fds[0]);
		signal(SIGUSR1,signal_func);
		wait(NULL);
		return 0;
	}
	close(fds[1]);
	pfactory pf;
	int pthread_num=16;
	int capacity=1000;
	factory_init(&pf,pthread_func,pthread_num,capacity);
	factory_start(pf);
	int sfd=tcp_init(ip,port);
	int epfd=epoll_create(1);
	struct epoll_event event,evs[2];
	event.events=EPOLLIN;
	event.data.fd=sfd;
	epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&event);
	event.data.fd=fds[0];
	epoll_ctl(epfd,EPOLL_CTL_ADD,fds[0],&event);
	int new_fd;
	int ret;
	listen(sfd,pthread_num);
	while(1){
		ret=epoll_wait(epfd,evs,2,-1);
		for(int i=0;i<ret;i++){
			if(evs[i].data.fd==sfd){
				new_fd=accept(sfd,NULL,NULL);
				pnode_t pnew=(pnode_t)calloc(1,sizeof(node_t));
				pnew->nd_sockfd=new_fd;
				pthread_mutex_lock(&pf->pq->que_mutex);
				factory_que_set(pf->pq,pnew);
				pthread_mutex_unlock(&pf->pq->que_mutex);
				pthread_cond_signal(&pf->cond);
			}
			if(evs[i].data.fd==fds[0]){
				close(sfd);
				epoll_ctl(epfd,EPOLL_CTL_DEL,sfd,&event);
				pnode_t new=(pnode_t)calloc(1,sizeof(node_t));
				new->nd_sockfd=-1;
				pthread_mutex_lock(&pf->pq->que_mutex);
				factory_que_destroy(pf->pq);
				factory_que_set(pf->pq,new);
				pthread_mutex_unlock(&pf->pq->que_mutex);
				pthread_cond_broadcast(&pf->cond);
				for(int j=0;j<pf->pthread_num;j++){              
					pthread_join(*(pf->pth_id+j),NULL);
				}
				return 0;
			}
		}
	}
}
