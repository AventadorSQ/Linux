/************************************************************************************************
 * udp服务端程序
 * *********************************************************************************************/

#include"udpsocket.hpp"

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		cout<<"./udp_srv ip port error"<<endl;
		return -1;
	}
	string ip = argv[1];
	uint16_t port = atoi(argv[2]);
	UdpSocket sock;

	//1.创建套接字
	CHECK_RET(sock.Socket());
	
	//2.绑定地址和端口
	CHECK_RET(sock.Bind(ip, port));

	//3.接收数据
	while(1)
	{
		string buf;
		struct sockaddr_in cli_addr;
		sock.Recv(buf, &cli_addr);
		cout<<"[server]client say:"<<buf.c_str()<<endl;

		//服务器回复客户端
		cout<<"[server]server say:";
		fflush(stdout);
		cin>>buf;
		sock.Send(buf, &cli_addr);
	}
	sock.Close();

	return 0;
}
