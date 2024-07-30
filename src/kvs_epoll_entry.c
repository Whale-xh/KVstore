#include "../include/kvs_epoll_entry.h"
#include "../include/kvstore.h"
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/poll.h>
#include <sys/epoll.h>
#include <sys/time.h>

#define EPOLL_ADD_FD 1
#define EPOLL_MOD_FD 0

#define TIME_SUB_MS(tv1, tv2) ((tv1.tv_sec - tv2.tv_sec) * 1000 + (tv1.tv_usec - tv2.tv_usec) / 1000)
struct timeval test_time;
int epfd = 0;
struct conn_item connlist[1048576] = {0}; // 1024  2G     2 * 128 * 1024 * 1024

int set_event(int fd, int event, int flag)
{
	if (flag)
	{ // 1 add, 0 mod
		struct epoll_event ev;
		ev.events = event;
		ev.data.fd = fd;
		epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	}
	else
	{
		struct epoll_event ev;
		ev.events = event;
		ev.data.fd = fd;
		epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
	}
}

int accept_cb(int fd)
{
	struct sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);
	int clientfd = accept(fd, (struct sockaddr *)&clientaddr, &len);
	if (clientfd < 0)
	{
		return -1;
	}
	set_event(clientfd, EPOLLIN, EPOLL_ADD_FD);
	connlist[clientfd].fd = clientfd;
	memset(connlist[clientfd].rbuffer, 0, BUFFER_LENGTH);
	connlist[clientfd].rlen = 0;
	memset(connlist[clientfd].wbuffer, 0, BUFFER_LENGTH);
	connlist[clientfd].wlen = 0;
	connlist[clientfd].recv_t.recv_callback = recv_cb;
	connlist[clientfd].send_callback = send_cb;
	if ((clientfd % 1000) == 999)
	{
		struct timeval tv_cur;
		gettimeofday(&tv_cur, NULL);
		int time_used = TIME_SUB_MS(tv_cur, test_time);

		memcpy(&test_time, &tv_cur, sizeof(struct timeval));

		printf("clientfd : %d, time_used: %d\n", clientfd, time_used);
	}
	return clientfd;
}

int recv_cb(int fd)
{ // fd --> EPOLLIN
	char *buffer = connlist[fd].rbuffer;
	// int idx = connlist[fd].rlen;
	int count = recv(fd, buffer, BUFFER_LENGTH, 0);
	if (count == 0)
	{
		printf("disconnect\n");
		epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
		close(fd);
		return -1;
	}
	connlist[fd].rlen = count;
#if 0 // echo: need to send
	memcpy(connlist[fd].wbuffer, connlist[fd].rbuffer, connlist[fd].rlen);
	connlist[fd].wlen = connlist[fd].rlen;
	connlist[fd].rlen -= connlist[fd].rlen;
#else
	kvstore_request(&connlist[fd]);
	connlist[fd].wlen = strlen(connlist[fd].wbuffer);
#endif
	set_event(fd, EPOLLOUT, EPOLL_MOD_FD);
	return count;
}

int send_cb(int fd)
{
	char *buffer = connlist[fd].wbuffer;
	int idx = connlist[fd].wlen;
	int count = send(fd, buffer, idx, 0);
	set_event(fd, EPOLLIN, EPOLL_MOD_FD);
	return count;
}

int init_server(unsigned short port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);
	if (-1 == bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)))
	{
		perror("bind");
		return -1;
	}
	listen(sockfd, 10);
	return sockfd;
}

// tcp
int epoll_entry()
{
	int port_count = 1;
	unsigned short port = 9096;
	epfd = epoll_create(1);
	for (int i = 0; i < port_count; i++)
	{
		int sockfd = init_server(port + i); // 9096,9097,9098...9116
		printf("Reactor =====> listen port : %d\n", port + i);
		connlist[sockfd].fd = sockfd;
		connlist[sockfd].recv_t.accept_callback = accept_cb;
		set_event(sockfd, EPOLLIN, 1);
	}
	gettimeofday(&test_time, NULL);
	struct epoll_event events[1024] = {0};
	while (1)
	{													 // mainloop();
		int nready = epoll_wait(epfd, events, 1024, -1); //
		int i = 0;
		for (i = 0; i < nready; i++)
		{
			int connfd = events[i].data.fd;
			if (events[i].events & EPOLLIN)
			{ //
				connlist[connfd].recv_t.recv_callback(connfd);
				// int count = connlist[connfd].recv_t.recv_callback(connfd);
				//  printf("recv count: %d <-- buffer: %s\n", count, connlist[connfd].rbuffer);
			}
			else if (events[i].events & EPOLLOUT)
			{
				connlist[connfd].send_callback(connfd);
				// printf("send --> buffer: %s\n",  connlist[connfd].wbuffer);
				// int count = connlist[connfd].send_callback(connfd);
			}
		}
	}
}