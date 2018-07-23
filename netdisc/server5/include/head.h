#ifndef __HEAD_H__
#define __HEAD_H__i

#include <openssl/md5.h>
#include <crypt.h>
#include <mysql/mysql.h>
#include <sys/sendfile.h>
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
#include <string.h>                                 
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

#define argu(a,b) {if(a!=b){printf("the argument's number is not right\n");return -1;}}
#define erro(a,b,c) {if(a==b){perror(c);}}
#endif
