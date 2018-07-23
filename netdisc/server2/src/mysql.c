#include "factory.h"

int query_users(int* puserid,char* name,char* salt,char* password,int* ppath)
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* passwd="1234";
	char* database="test";
	char query[512]="select ID,SALT,PASSWORD,PATH from users where NAME='";
	sprintf(query,"%s%s%s",query,name,"'");
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,passwd,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res);
		mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else{
		res=mysql_use_result(conn);
		if(res)
		{
			if((row=mysql_fetch_row(res))!=NULL)
			{
				*puserid=atoi(row[0]);
				strcpy(salt,row[1]);
				strcpy(password,row[2]);
				*ppath=atoi(row[3]);
			}
			else{
				return -1;
			}
		}	
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

int query_code(int procode,int* code)
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* passwd="1234";
	char* database="test";
	char query[512]="select CODE from file where PROCODE=";
	sprintf(query,"%s%d",query,procode);
	int t;
	int i=0;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,passwd,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res);
		mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else
	{
		res=mysql_use_result(conn);
		if(res)
		{
			if((row=mysql_fetch_row(res))==NULL)
			{
				return -1;
			}
			do
			{
				code[i++]=atoi(row[0]);
			}
			while((row=mysql_fetch_row(res))!=NULL);
		}	
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

int query_procode(int* procode,int code)
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* passwd="1234";
	char* database="test";
	char query[512]="select PROCODE from file where CODE=";
	sprintf(query,"%s%d",query,code);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,passwd,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res);
		mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else
	{
		res=mysql_use_result(conn);
		if(res)
		{
			if((row=mysql_fetch_row(res))!=NULL)
			{
				*procode=atoi(row[0]);	
			}
			else
			{
				return -1;
			}
		}	
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

int query_md5(char* md5)
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	int num=0;
	char* server="localhost";
	char* user="root";
	char* password="1234";
	char* database="test";
	char query[512]="select * from file where MD5='";
	sprintf(query,"%s%s%s",query,md5,"'");
	conn=mysql_init(NULL);
	mysql_real_connect(conn,server,user,password,database,0,NULL,0);
	mysql_query(conn,query);
	res=mysql_use_result(conn);
	if(res)
	{
		if((row=mysql_fetch_row(res))==NULL)
		{
			return -1;
		}
		do
		{
			num++;
		}
		while((row=mysql_fetch_row(res))!=NULL);
	}	
	mysql_free_result(res);
	mysql_close(conn);
	return num;
}

int query_md5id(char* md5,int userid)
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="1234";
	char* database="test";
	char query[512]="select * from file where MD5='";
	sprintf(query,"%s%s%s%d",query,md5,"' and ID= ",userid);
	conn=mysql_init(NULL);
	mysql_real_connect(conn,server,user,password,database,0,NULL,0);
	mysql_query(conn,query);
	res=mysql_use_result(conn);
	if(res)
	{
		if((row=mysql_fetch_row(res))!=NULL)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}	
	mysql_free_result(res);
	mysql_close(conn);
	return 0;
}

int query_file(int path,int userid,char* buf)
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="1234";
	char* database="test";
	char query[512]="select FILENAME,TYPE,SIZE from file where PROCODE=";
	sprintf(query,"%s%d%s%d",query,path," and ID=",userid);
	sprintf(buf,"%-14s%-14s%s","[FILENAME]","[TYPE]","[SIZE]\n");
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res);
		mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else{
		res=mysql_use_result(conn);
		if(res)
		{
			if((row=mysql_fetch_row(res))==NULL){
				sprintf(buf,"%s%-14s%-14s%-14s",buf,"NULL","NULL","NULL");
				return 0;
			}
			do
			{
				for(int i=0;i<mysql_num_fields(res);i++)
				{
					sprintf(buf,"%s%-14s",buf,row[i]);
				}
				sprintf(buf,"%s%s",buf,"\n");
			}while((row=mysql_fetch_row(res))!=NULL);
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

int query_path(int* ppath,char* filename,int userid)
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="1234";
	char* database="test";
	char query[512]="select CODE from file where FILENAME='";
	sprintf(query,"%s%s%s%d",query,filename,"' and ID=",userid);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res);
		mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else{
		res=mysql_use_result(conn);
		if(res)
		{
			while((row=mysql_fetch_row(res))!=NULL)
			{
				*ppath=atoi(row[0]);
			}
		}
		mysql_free_result(res);
		mysql_close(conn);
		return 0;
	}
	return 0;
}

int query_filename(int path,char (*filename)[1024],int userid)
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="1234";
	char* database="test";
	char query[200]="select FILENAME from file where PROCODE=";
	sprintf(query,"%s%d%s%d",query,path," and ID=",userid);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res);
		mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else{
		res=mysql_use_result(conn);
		if(res)
		{
			int i=0;
			while((row=mysql_fetch_row(res))!=NULL)
			{
				strcpy(filename[i],row[0]);
				i++;
			}
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

int query_puts(char* filename,char* md5,off_t* size,int userid)
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row=0;
	char* server="localhost";
	char* user="root";
	char* password="1234";
	char* database="test";
	char query[512]="select MD5,SIZE from file where FILENAME='";
	sprintf(query,"%s%s%s%d",query,filename,"' and ID=",userid);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res);
		mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else{
		res=mysql_use_result(conn);
		if(res)
		{
			if((row=mysql_fetch_row(res))==NULL){
				return -1;
			}
			do
			{
				strcpy(md5,row[0]);
				*size=atoi(row[1]);
			}while((row=mysql_fetch_row(res))!=NULL);
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

int query_md5torm(char* md5,int code)
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="1234";
	char* database="test";
	char query[512]="select MD5 from file where CODE=";
	sprintf(query,"%s%d",query,code);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
		mysql_free_result(res);
		mysql_close(conn);
		return -1;
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	else{
		res=mysql_use_result(conn);
		if(res)
		{
			while((row=mysql_fetch_row(res))!=NULL)
			{
				if(!row[0]){
					return -1;
				}
				strcpy(md5,row[0]);
			}
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}


int query_pwd(int path,char* buf)
{
	MYSQL *conn;
	MYSQL_RES *res=0;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="1234";
	char* database="test";
	char tmp[10][1024]={0};
	conn=mysql_init(NULL);
	mysql_real_connect(conn,server,user,password,database,0,NULL,0);
	int k=0;
	while(path!=0){
		char query[512]="select PROCODE,FILENAME from file where CODE=";
		sprintf(query,"%s%d",query,path);
		mysql_query(conn,query);
		res=mysql_use_result(conn);
		if(res)
		{
			while((row=mysql_fetch_row(res))!=NULL)
			{
				path=atoi(row[0]);
				sprintf(tmp[k++],"%s%s",row[1],"/");
			}

		}
	}
	strcpy(buf,"/home/");
	for(int i=k-1;i>=0;i--){
		sprintf(buf,"%s%s",buf,tmp[i]);
	}
	mysql_free_result(res);
	mysql_close(conn);
	return 0;
}

int insert_users(char* name,char* salt,char* password,int path)
{
	MYSQL *conn;
	char* server="localhost";
	char* user="root";
	char* passwd="1234";
	char* database="test";
	char query[200]="insert into users(NAME,SALT,PASSWORD,PATH) values('";
	sprintf(query,"%s%s%s%s%s%s%s%d%s",query,name,"','",salt,"','",password,"',",path,")");
	conn=mysql_init(NULL);
	mysql_real_connect(conn,server,user,passwd,database,0,NULL,0);
	mysql_query(conn,query);
	mysql_close(conn);
	return 0;
}

int insert_dir(int path,int userid,char* name)
{
	MYSQL *conn;
	char* server="localhost";
	char* user="root";
	char* password="1234";
	char* database="test";
	char query[200]="insert into file(PROCODE,ID,FILENAME,TYPE,SIZE) values(";
	sprintf(query,"%s%d%s%d%s%s%s",query,path,",",userid,",'",name,"','d',8)");
	conn=mysql_init(NULL);
	mysql_real_connect(conn,server,user,password,database,0,NULL,0);
	mysql_query(conn,query);
	mysql_close(conn);
	return 0;
}

int insert_file(int path,int userid,char* name,char* md5,long long size)
{
	MYSQL *conn;
	char* server="localhost";
	char* user="root";
	char* password="1234";
	char* database="test";
	char query[256]="insert into file(PROCODE,ID,FILENAME,TYPE,MD5,SIZE) values(";
	sprintf(query,"%s%d%s%d%s%s%s%s%s%lld%s",query,path,",",userid,",'",name,"','f','",md5,"',",size,")");
	conn=mysql_init(NULL);
	mysql_real_connect(conn,server,user,password,database,0,NULL,0);
	mysql_query(conn,query);
	mysql_close(conn);
	return 0;
}

int delete_code(int path)
{
	int core[16]={0};
	if(!query_code(path,core)){
		int num=0;
		while(core[num]){
			delete_code(core[num++]);
		}
	}
	MYSQL *conn;
	char* server="localhost";
	char* user="root";
	char* password="1234";
	char* database="test";
	char query[200]="delete from file where CODE=";
	sprintf(query,"%s%d",query,path);
	int t;
	char md5[512]={0};
	char tmp[1024]={0};
	if(query_md5torm(md5,path)==0&&query_md5(md5)==1){
		sprintf(tmp,"%s%s","rm ",md5);
		system(tmp);
	}
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	mysql_close(conn);
	return 0;
}


int changpath(int userid,int path)
{
	MYSQL *conn;
	char* server="localhost";
	char* user="root";
	char* password="1234";
	char* database="test";
	char query[512]="update users set PATH=";
	sprintf(query,"%s%d%s%d",query,path," where ID=",userid);
	int t;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0))
	{
		printf("Error connecting to database:%s\n",mysql_error(conn));
	}
	t=mysql_query(conn,query);
	if(t)
	{
		printf("Error making query:%s\n",mysql_error(conn));
	}
	mysql_close(conn);
	return 0;
}

