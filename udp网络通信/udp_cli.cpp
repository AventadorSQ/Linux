/*********************************************************************************************
 * udpsocket实现udp客户端程序
 * ******************************************************************************************/
#include"udpsocket.hpp"

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		cout<<"./udp_cli ip port error"<<endl;
		return -1;
	}
	string ip = argv[1];
	uint16_t port = atoi(argv[2]);
	//uint16_t port = atoi(9999);
	struct sockaddr_in srv_addr;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(port);
	//srv_addr.sin_port = htons(9999);
	srv_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	//srv_addr.sin_addr.s_addr = inet_addr("192.168.166.142");

	UdpSocket sock;
	//1.创建socket
	CHECK_RET(sock.Socket());

	//2.发送数据
	while(1)
	{
		string buf;
		cout<<"[client1]client say:";
		fflush(stdout);
		cin>>buf;
		sock.Send(buf, &srv_addr);

		buf.clear();
		sock.Recv(buf);
		cout<<"[client1]server say:"<<buf<<endl;
	}
	sock.Close();
	return 0;
}
