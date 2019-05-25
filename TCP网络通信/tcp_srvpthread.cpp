/*****************************************************************************************
 * Tcp服务端程序
 * *************************************************************************************/

#include"tcpsocket.hpp"
#include<pthread.h>

void* pthread_start(void* arg)
{
	TcpSocket* client = (TcpSocket*)arg;
	while(1)
	{
	string buf;
	client->Recv(buf);
	cout<<"[server]client say:"<<buf<<endl;

	cout<<"[server]server say:";
	fflush(stdout);
	cin>>buf;
	client->Send(buf);
	}
	delete client;
	return NULL;
}

int main(int argc, char* argv[])
{
	TcpSocket sock;

	if(argc != 3)
	{
		cout<<"./tcp_srv ip port error"<<endl;
		return -1;
	}
	string ip = argv[1];
	uint16_t port = atoi(argv[2]);

	CHECK_RET(sock.Socket());
	CHECK_RET(sock.Bind(ip,port));

	CHECK_RET(sock.Listen());

	while(1)
	{
		//程序卡在accept是因为用户无法得知客户端的新连接什么时候到来
		////如果能知道什么时候到来就不会阻塞；只需再来的时候调用一次就行
		TcpSocket* client = new TcpSocket();
		if(sock.Accept(*client) == false)
		{
			continue;
		}
		pthread_t tid;
		pthread_create(&tid, NULL, pthread_start, (void*)client);
		pthread_detach(tid);
	}
	sock.Close();
	return 0;
}
