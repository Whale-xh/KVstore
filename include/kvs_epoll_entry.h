#ifndef __KVS_EPOLL_ENTRY_H__
#define __KVS_EPOLL_ENTRY_H__


int set_event(int fd, int event, int flag);
int accept_cb(int fd);
int recv_cb(int fd);
int send_cb(int fd);
int init_server(unsigned short port);
int epoll_entry();

#endif