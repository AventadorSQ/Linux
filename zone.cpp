/*演示父进程创建子进程之后，父子进程数据独有
 */
#include<unistd.h>
#include<iostream>
using namespace std;

int val = 100;
int main()
{
int pid=fork();
if(pid == 0)
{
val=200;
cout<<"child val:"<<val<<" "<<&val<<endl;
}
else if(pid > 0)
{
cout<<"parent val:"<<val<<" "<<&val<<endl;
}
return 0;
}
