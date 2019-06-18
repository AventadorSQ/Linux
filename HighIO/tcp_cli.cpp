/*************************************************************************************************
 * 调用TcpSocket类的相关函数实现客户端程序
 * *********************************************************************************************/

#include"tcpsocket.hpp"
#include<signal.h>

void sigcb(int n)
{
	cout<<"recv signo:"<<n<<endl;
}

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		cout<<"./tcp_cli ip port error"<<endl;
		return -1;
	}
	signal(SIGPIPE, sigcb);

	string ip = argv[1];
	uint16_t port = atoi(argv[2]);

	TcpSocket sock;
	CHECK_RET(sock.Socket());
	CHECK_RET(sock.Connect(ip, port));

	while(1)
	{
		string buf;
		cout<<"[client]client say:";
		fflush(stdout);
		cin>>buf;
		sock.Send(buf);

		buf.clear();
		sock.Recv(buf);
		cout<<"[client]server say:"<<buf<<endl;
	}
	return 0;
}
