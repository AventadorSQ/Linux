#include<iostream>
#include<unistd.h>
using namespace std;

int main()
{
	if(fork() && fork() || fork())
	{
		fork();
	}
	while(1)
	{
		sleep(1);
	}
	return 0;
}
