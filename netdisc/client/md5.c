#include "func.h"

int get_md5(char* filename,char* md5) 
{
	MD5_CTX ctx;
	unsigned char outmd[16];
	char buffer[1024];
	int len=0;
	int i=0;
	FILE * fp=NULL;
	memset(outmd,0,sizeof(outmd));
	memset(buffer,0,sizeof(buffer));
	fp=fopen(filename,"rb");
	if(fp==NULL)
	{
		printf("the file not exist in client!!!\n");
		return -1;
	}

	MD5_Init(&ctx);
	while((len=fread(buffer,1,1024,fp))>0)
	{
		MD5_Update(&ctx,buffer,len);
		memset(buffer,0,sizeof(buffer));
	}
	MD5_Final(outmd,&ctx);
	for(i=0;i<16;i++)
	{
		sprintf(md5,"%s%02X",md5,outmd[i]);
	}
	fclose(fp);
	return 0;
}  

