/*环境变量
*/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main(int argc, char *argv[], char* env[])
{
    //char *getenv(const char *name);
    //通过环境变量名称获取环境变量的值
    char* ptr=getenv("MYVAL");
    printf("MYVAL:%s\n",ptr);

    int i;
    for(i = 0;env[i]!=NULL;i++)
    {
	printf("env[%d]:[%s]\n",i,env[i]);
    }

    extern char** environ;
    for(i = 0;environ[i]!=NULL;i++)
    {
	printf("environ[%d]:[%s]\n",i,environ[i]);
    }
    return 0;
}
