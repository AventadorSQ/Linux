/*验证进程终止
 * */
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<iostream>
using namespace std;
int a=1;
int main()
{
cout<<"hehehe@@@";
//void _exit(int status);
//_exit(666);
//void exit(int status);
//正常终止进程，并且返回val
exit(253);
    for(int i=0;i<255;i++)
    {
	//char *strerror(int errnum);
	//根据错误编号获取错误信息
	cout<<"errno:"<<i<<":"<<strerror(i)<<endl;
    }
    if(a==1)
    {
	return 256;
    }
    else
    {
	return -1;
    }
    while(1)
    {
	cout<<"haha<--->hehe"<<endl;
	sleep(1);
    }
}
