#include<iostream>
#include<sys/epoll.h>
#include<vector>
#include"tcpsocket.hpp"
using namespace std;

class Epoll
{
	public:
		bool Init()
		{
			//int epoll_create(int size);
			//创建epoll;返回操作句柄
			_epfd = epoll_create(6);
			if(_epfd < 0)
			{
				perror("epoll create error");
				return false;
			}
			return true;
		}

		bool Add(TcpSocket& sock, uint32_t events = EPOLLIN)
		{
			struct epoll_event ev;
			int fd = sock.GetFd();
			ev.events = events;
			ev.data.fd = fd;
			//int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
			int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev);
			if(ret < 0)
			{
				perror("epoll ctrol error");
				return false;
			}
			return true;
		}

		bool Del(TcpSocket& sock)
		{
			int fd = sock.GetFd();
			epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL);
		}

		bool Wait(vector<TcpSocket>& _list, int timeout = 3000)
		{
			//int epoll_wait(int epfd, struct epoll_event *events,int maxevents, int timeout);
			struct epoll_event evs[10];
			int nfds = epoll_wait(_epfd, evs, 10, timeout);
			if(nfds < 0)
			{
				perror("epoll wait error");
				return false;
			}
			else if(nfds == 0)
			{
				cout<<"epoll wait timeout"<<endl;
				return false;
			}

			for(int i = 0; i < nfds; i++)
			{
				TcpSocket sock;
				sock.SetFd(evs[i].data.fd);
				_list.push_back(sock);
				return true;
			}
		}
	private:
		int _epfd;
};

int main()
{
	TcpSocket lst_sock;

	CHECK_RET(lst_sock.Socket());
	CHECK_RET(lst_sock.Bind("0.0.0.0", 9000));
	CHECK_RET(lst_sock.Listen());

	Epoll epoll;
	epoll.Init();
	epoll.Add(lst_sock);

	while(1)
	{
		vector<TcpSocket> list;
		bool ret = epoll.Wait(list);
		if(ret == false)
		{
			continue;
		}
		for(int i = 0; i < list.size(); i++)
		{
			if(list[i].GetFd() == lst_sock.GetFd())
			{
				TcpSocket cli_sock;
				ret = lst_sock.Accept(cli_sock);
				if(ret == false)
				{
					continue;
				}
				epoll.Add(cli_sock);
			}
			else
			{
				string buf;
				ret = list[i].Recv(buf);
				if(ret == false)
				{
					epoll.Del(list[i]);
					list[i].Close();
				}
				cout<<"client say:"<<buf<<endl;
			}
		}

	}
	lst_sock.Close();
	return 0;
}
