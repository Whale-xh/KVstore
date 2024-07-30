#ifndef __KVSTORE_H__
#define __KVSTORE_H__
#include<stddef.h>

#define BUFFER_LENGTH 512

typedef int (*RCALLBACK)(int fd);

struct conn_item
{
    int fd;
    char rbuffer[BUFFER_LENGTH];
    char wbuffer[BUFFER_LENGTH];
    int rlen;
    int wlen;
    union
    {
        RCALLBACK accept_callback;
        RCALLBACK recv_callback;
    } recv_t;
    RCALLBACK send_callback;
};

int kvstore_request(struct conn_item *item);
void *kvstore_malloc(size_t size);
void kvstore_free(void *ptr);
int kvstore_split_token(char *msg, char **tokens);
int kvstore_parser_protocol(struct conn_item *item, char **tokens, int count);
int init_kvengine();
int exit_kvengine();



#endif