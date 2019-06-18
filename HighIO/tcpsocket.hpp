/***********************************************************************************************
 * 封装TcpSocket类，提供轻便的socket接口
 * ********************************************************************************************/

#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<string>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
using namespace std;

#define CHECK_RET(q) if((q) == false){return -1;}
class TcpSocket
{
	public:
		TcpSocket()
			:_sockfd(-1)
		{}
		int GetFd()
		{
			return _sockfd;
		}

		//1.创建套接字
		bool Socket()
		{
			_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if(_sockfd < 0)
			{
				cout<<"socket error"<<endl;
				return false;
			}
			return true;
		}

		//2.绑定ip和port
		bool Bind(const string& ip, const uint16_t port)
		{
			//int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			addr.sin_addr.s_addr = inet_addr(ip.c_str());

			socklen_t len = sizeof(struct sockaddr_in);

			int ret = bind(_sockfd, (struct sockaddr*)&addr, len);
			if(ret < 0)
			{
				cout<<"bind error"<<endl;
				return false;
			}
			return true;
		}
		//2服务端
		//2.1开始监听
	    bool Listen(int baklog = 5)
		{
			//int listen(int sockfd, int backlog);
			//sockfd: 套接字描述符
			//backlog: backlog设置内核中已完成连接队列的最大节点数量
			int ret = listen(_sockfd, baklog);
			if(ret < 0)
			{
				cout<<"listen error"<<endl;
				return false;
			}
			return true;
		}
		//2.2服务器获取连接成功的客户端的socket,并且返回客户端的地址信息
		bool Accept(TcpSocket& sock, sockaddr_in* addr = NULL)
		{
			//int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
			//addr:      用于存储客户端地址信息
			//addrlen:   用于设置想要的地址长度和保存实际的地址长度
			//返回值：  为客户端连接新建的socket描述符， 失败-1
			//接下来与客户端的通信就是通过这个socket描述符实现的
			struct sockaddr_in _addr;
			socklen_t len = sizeof(struct sockaddr_in);
			int newfd = accept(_sockfd, (struct sockaddr*)&_addr, &len);
			if(newfd < 0)
			{
				cout<<"accept error"<<endl;
				return false;
			}
			sock.SetFd(newfd);
			if(addr != NULL)
			{
				memcpy(addr, &_addr, len);
			}

			return true;
		}
		//3.客户端向服务端发起连接请求
		bool Connect(string& ip, uint16_t port)
		{
			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port = htons(port);
			addr.sin_addr.s_addr = inet_addr(ip.c_str());
			socklen_t len = sizeof(addr);
			//int connect(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
			//addr:服务端监听地址信息
			//addlen： 地址信息长度
			int ret = connect(_sockfd, (struct sockaddr*)&addr, len);
			if(ret < 0 )
			{
				cout<<"connect error"<<endl;
				return false;
			}
			return true;
		}
		//4.开始通信---tcp服务端也可以直接发送数据（因为连接已经建立成功）
		ssize_t Recv(string& buf)
		{
			//ssize_t recv(int sockfd, void *buf, size_t len, int flags);
			//buf: 接收的数据
			//len：想要接收数据的长度
			//flags：0-阻塞接收 
			//       MSG_PEEK-接收数据但是不把数据从接收缓冲区移除
			//返回值： 实际接收的字节数，对端关闭连接返回0， 错误返回-1
			char arr[1024] = {0};
			int ret = recv(_sockfd, arr, 1024, 0);
			if(ret == 0)
			{
				cout<<"peer shutdown"<<endl;
				return false;
			}
			else if(ret < 0)
			{
				cout<<"recv error"<<endl;
				return false;
			}
			buf.assign(arr, ret);
			return true;
		}
		//若连接断开，触发异常
		bool Send(string& buf)
		{
			//ssize_t send(int sockfd, const void *buf, size_t len, int flags);
			//返回值：实际发送的字节数  失败：-1
			int ret = send(_sockfd, buf.c_str(), buf.size(), 0);
			if(ret < 0)
			{
				cout<<"send error"<<endl;
				return false;
			}
			return true;
		}
		//5.关闭
		bool Close()
		{
			close(_sockfd);
			_sockfd = -1;
			return true;
		}
		void SetFd(int sockfd)
		{
			_sockfd = sockfd;
		}
	private:
		int _sockfd;
};
