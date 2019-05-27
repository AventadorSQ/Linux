/*****************************************************************************************************
 * 封装一个udp的套接字接口类
 ****************************************************************************************************/

#include<iostream>
#include<errno.h>
#include<string>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

using namespace std;

#define CHECK_RET(q) if((q) == false){return -1;}

class UdpSocket
{
	public:
		UdpSocket()
		{}
		~UdpSocket()
		{}
		//1.创建socket套接字
		bool Socket()
		{
			//int socket(int domain, int type, int protocol);
			// domain:地址域
			//    AF_INET  表示使用IPV4网络协议
			// type:  套接字类型
			//    SOCK_STREAM 流式套接字---默认协议TCP
			//    SOCK_DGRAM  数据报套接字-默认协议UDP
			// protocol:
			//    0  使用默认协议
			//    IPPROTO_TCP 6  TCP协议
			//    IPPROTO_UDP 17 UDP协议
			// 返回值：套接字操作句柄-文件描述符  失败：-1
		    _sock = socket(AF_INET, SOCK_DGRAM, 0);
			if(_sock < 0)
			{
				perror("create socket error");
				return false;
			}
			return true;
		}
		//2.为套接字绑定地址信息
		bool Bind(string& ip, uint16_t port)
		{
			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			//因为网络通信使用网络字节序，因此端口和ip地址信息都需要被转换为网络字节序的数据
			//uint16_t htons(uint16_t hostshort);
			//  将16位的数据从主机字节序转换为网络字节序
			//uint32_t htonl(uint32_t hostlong);
			//  将32位的数据从主机字节序转换位网络字节序
			addr.sin_port = htons(port);
			// in_addr_t inet_addr(const char *cp);
			// 将字符串点分十进制IP地址转换位网络字节序的整数IP地址
			addr.sin_addr.s_addr = inet_addr(ip.c_str());
			//addr.sin_addr.s_addr = htonl(0xc0a8a688);
			//int inet_pton(int af, const char *src, void *dst);
			//inst_pton(AF_INET, ip.c_str(), &addr);

			//int bind(int sockfd, const struct sockaddr *addr,socklen_t addrlen);
			// sockfd: 创建套接字返回的套接字描述符
			// addr:   要绑定的地址信息
			// addrlen： 地址信息长度
			// 返回值：0    失败：-1
			socklen_t len = sizeof(struct sockaddr_in);
			int ret = bind(_sock, (struct sockaddr*)&addr, len);
			if(ret < 0)
			{
				perror("bind error");
				return -1;
			}
			return true;
		}
		//3.接受数据
		bool Recv(string& buf, sockaddr_in* _addr)
		{
			//ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
			//struct sockaddr *src_addr, socklen_t *addrlen);
			//  sockfd: 套接字描述符
			//  buf:     用于保存接受的数据
			//  len:    要接收的数据长度
			//  flags:  默认0---阻塞接收---没有数据则阻塞
			//  src_addr:  发送端的地址信息
			//  addrlen:   输入输出型参数，用于获取地址长度
			//  返回值： 实际接收的数据长度  失败：-1
			char tmp[1024] = {0};
			struct sockaddr_in addr;
			socklen_t len = sizeof(struct sockaddr_in);
			int ret = recvfrom(_sock, tmp, 1024, 0, (struct sockaddr*)&addr, &len);
			if(ret < 0)
			{
				perror("recvfrom error");
				return false;
			}
			buf.assign(tmp, ret);
			if(_addr != NULL)
			{
				memcpy(_addr, &addr, len);
			}

			return true;
		}
		//4.发送数据
		bool Send(string& buf, struct sockaddr_in* addr)
		{
			//ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen);
			// dest_addr: 目的端地址信息
			// addlen: 地址信息长度
			socklen_t len = sizeof(struct sockaddr_in);
			ssize_t r = sendto(_sock, buf.c_str(), buf.size(), 0, (struct sockaddr *)addr, len);
			if (r == -1) {
				printf("%d, %d\n", errno, EAFNOSUPPORT);
				perror("sendto error");
				return false;
			}
			return true;
		}
		//5.关闭套接字
		bool Close()
		{
			close(_sock);
			_sock = -1;
		}
	private:
		int _sock;
};
