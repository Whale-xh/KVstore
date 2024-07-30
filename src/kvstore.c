#include "../include/kvstore.h"
#include "../include/kvs_array.h"
#include "../include/kvs_hash.h"
#include "../include/kvs_rbtree.h"
#include "../include/kvs_waveco_entry.h"
#include "../include/kvs_epoll_entry.h"
//#include "../include/kvs_io_uring_entry.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define ENABLE_ARRAY_KVENGINE 1
#define ENABLE_RBTREE_KVENGINE 1
#define ENABLE_HASH_KVENGINE 1

#define NETWORK_EPOLL 0
#define NETWORK_WAVECO 1
#define NETWORK_IOURING 2

#define ENABLE_NETWORK_SELECT NETWORK_WAVECO    //network select

#define KVSTORE_MAX_TOKENS 8

const char *commands[] = {
    "SET", "GET", "DEL", "MOD", "COUNT",
    "RSET", "RGET", "RDEL", "RMOD", "RCOUNT",
    "HSET", "HGET", "HDEL", "HMOD", "HCOUNT"
};

enum
{
    KVS_CMD_START = 0,

    KVS_CMD_SET = KVS_CMD_START,
    KVS_CMD_GET,
    KVS_CMD_DEL,
    KVS_CMD_MOD,
    KVS_CMD_COUNT,

    KVS_CMD_RSET,
    KVS_CMD_RGET,
    KVS_CMD_RDEL,
    KVS_CMD_RMOD,
    KVS_CMD_RCOUNT,

    KVS_CMD_HSET,
    KVS_CMD_HGET,
    KVS_CMD_HDEL,
    KVS_CMD_HMOD,
    KVS_CMD_HCOUNT,

    KVS_CMD_SIZE
};

int kvstore_request(struct conn_item *item)
{
    // printf("recv:%s\n",item->rbuffer);
    char *msg = item->rbuffer;
    char *tokens[KVSTORE_MAX_TOKENS] = {NULL};
    int count = kvstore_split_token(msg, tokens);
    kvstore_parser_protocol(item, tokens, count);
    return 0;
}

void *kvstore_malloc(size_t size)
{
    return malloc(size);
}

void kvstore_free(void *ptr)
{
    return free(ptr);
}

#if ENABLE_ARRAY_KVENGINE

int kvstore_array_set(char *key, char *value)
{
    return array_set(&kvs_Array, key, value);
}
char *kvstore_array_get(char *key)
{
    return array_get(&kvs_Array, key);
}
int kvstore_array_delete(char *key)
{
    return array_del(&kvs_Array, key);
}
int kvstore_array_modify(char *key, char *value)
{
    return array_modify(&kvs_Array, key, value);
}
int kvstore_array_count()
{
    return array_count(&kvs_Array);
}

#endif

#if ENABLE_RBTREE_KVENGINE
int kvstore_rbtree_set(char *key, char *value)
{
    return rbtree_set(&kvs_Tree, key, value);
}

char *kvstore_rbtree_get(char *key)
{
    return rbtree_get(&kvs_Tree, key);
}

int kvstore_rbtree_delete(char *key)
{
    return rbtree_del(&kvs_Tree, key);
}

int kvstore_rbtree_modify(char *key, char *value)
{
    return rbtree_modify(&kvs_Tree, key, value);
}

int kvstore_rbtree_count()
{
    return rbtree_count(&kvs_Tree);
}
#endif

#if ENABLE_HASH_KVENGINE

int kvstore_hash_set(char *key, char *value)
{
    return hash_set(&kvs_Hash, key, value);
}
char *kvstore_hash_get(char *key)
{
    return hash_get(&kvs_Hash, key);
}
int kvstore_hash_delete(char *key)
{
    return hash_delete(&kvs_Hash, key);
}
int kvstore_hash_modify(char *key, char *value)
{
    return hash_modify(&kvs_Hash, key, value);
}
int kvstore_hash_count(void)
{
    return hash_count(&kvs_Hash);
}

#endif

int kvstore_split_token(char *msg, char **tokens)
{
    if (msg == NULL || tokens == NULL)
        return -1;
    int idx = 0;
    char *token = strtok(msg, " ");
    while (token != NULL)
    {
        tokens[idx++] = token;
        token = strtok(NULL, " ");
    }
    return idx;
}

int kvstore_parser_protocol(struct conn_item *item, char **tokens, int count)
{
    if (item == NULL || tokens[0] == NULL || count == 0)
        return -1;
    int cmd = KVS_CMD_START;
    for (cmd = KVS_CMD_START; cmd < KVS_CMD_SIZE; ++cmd)
    {
        if (strcmp(commands[cmd], tokens[0]) == 0)
        {
            break;
        }
    }
    char *msg = item->wbuffer;
    memset(msg, 0, BUFFER_LENGTH);
    char *key = tokens[1];
    char *value = tokens[2];
    switch (cmd)
    {
    // array
    case KVS_CMD_SET:
    {
        int ret = kvstore_array_set(key, value);
        if (!ret)
        {
            snprintf(msg, BUFFER_LENGTH, "SUCCESS");
        }
        else
        {
            snprintf(msg, BUFFER_LENGTH, "FAILED");
        }
        break;
    }
    case KVS_CMD_GET:
    {
        char *ret = kvstore_array_get(key);
        if (ret)
        {
            snprintf(msg, BUFFER_LENGTH, "%s", ret);
        }
        else
        {
            snprintf(msg, BUFFER_LENGTH, "NO EXIST");
        }
        break;
    }
    case KVS_CMD_DEL:
    {
        int ret = kvstore_array_delete(key);
        if (ret < 0)
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
        }
        else if (ret == 0)
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");
        }
        else
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "NO EXIST");
        }
        break;
    }
    case KVS_CMD_MOD:
    {
        int ret = kvstore_array_modify(key, value);
        if (ret < 0)
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
        }
        else if (ret == 0)
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");
        }
        else
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "NO EXIST");
        }
        break;
    }
    case KVS_CMD_COUNT:
    {
        int ret = kvstore_array_count();
        if (ret < 0)
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
        }
        else
        {
            snprintf(msg, BUFFER_LENGTH, "%d", ret);
        }
        break;
    }
    // rbtree
    case KVS_CMD_RSET:
    {
        int ret = kvstore_rbtree_set(key, value);
        if (!ret)
        {
            snprintf(msg, BUFFER_LENGTH, "SUCCESS");
        }
        else
        {
            snprintf(msg, BUFFER_LENGTH, "FAILED");
        }
        break;
    }
    case KVS_CMD_RGET:
    {
        char *ret = kvstore_rbtree_get(key);
        if (ret)
        {
            snprintf(msg, BUFFER_LENGTH, "%s", ret);
        }
        else
        {
            snprintf(msg, BUFFER_LENGTH, "NO EXIST");
        }
        break;
    }
    case KVS_CMD_RDEL:
    {
        int ret = kvstore_rbtree_delete(key);
        if (ret < 0)
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
        }
        else if (ret == 0)
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");
        }
        else
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "NO EXIST");
        }
        break;
    }
    case KVS_CMD_RMOD:
    {
        int ret = kvstore_rbtree_modify(key, value);
        if (ret < 0)
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
        }
        else if (ret == 0)
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");
        }
        else
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "NO EXIST");
        }
        break;
    }
    case KVS_CMD_RCOUNT:
    {
        int ret = kvstore_rbtree_count();
        if (ret < 0)
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
        }
        else
        {
            snprintf(msg, BUFFER_LENGTH, "%d", ret);
        }
        break;
    }
    case KVS_CMD_HSET:
    {

        int ret = kvstore_hash_set(key, value);
        if (!ret)
        {
            snprintf(msg, BUFFER_LENGTH, "SUCCESS");
        }
        else
        {
            snprintf(msg, BUFFER_LENGTH, "FAILED");
        }
        break;
    }
    // hash
    case KVS_CMD_HGET:
    {

        char *ret = kvstore_hash_get(key);
        if (ret)
        {
            snprintf(msg, BUFFER_LENGTH, "%s", ret);
        }
        else
        {
            snprintf(msg, BUFFER_LENGTH, "NO EXIST");
        }

        break;
    }
    case KVS_CMD_HDEL:
    {

        int ret = kvstore_hash_delete(key);
        if (ret < 0)
        { // server
            snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
        }
        else if (ret == 0)
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");
        }
        else
        {
            snprintf(msg, BUFFER_LENGTH, "NO EXIST");
        }

        break;
    }
    case KVS_CMD_HMOD:
    {

        int ret = kvstore_hash_modify(key, value);
        if (ret < 0)
        { // server
            snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
        }
        else if (ret == 0)
        {
            snprintf(msg, BUFFER_LENGTH, "%s", "SUCCESS");
        }
        else
        {
            snprintf(msg, BUFFER_LENGTH, "NO EXIST");
        }
        break;
    }
    case KVS_CMD_HCOUNT:
    {
        int ret = kvstore_hash_count();
        if (ret < 0)
        { // server
            snprintf(msg, BUFFER_LENGTH, "%s", "ERROR");
        }
        else
        {
            snprintf(msg, BUFFER_LENGTH, "%d", ret);
        }
        break;
    }
    default:
    {
        assert(0);
    }
    }
}



int init_kvengine()
{
#if ENABLE_ARRAY_KVENGINE
    kvstore_array_create(&kvs_Array);
#endif
#if ENABLE_RBTREE_KVENGINE
    kvstore_rbtree_create(&kvs_Tree);
#endif
#if ENABLE_HASH_KVENGINE
    kvstore_hash_create(&kvs_Hash);
#endif
}

int exit_kvengine()
{
#if ENABLE_ARRAY_KVENGINE
    kvstore_array_destory(&kvs_Array);
#endif
#if ENABLE_RBTREE_KVENGINE
    kvstore_rbtree_destory(&kvs_Tree);
#endif
#if ENABLE_HASH_KVENGINE
    kvstore_hash_destory(&kvs_Hash);
#endif
}

int main(int argc, char *argv[])
{
    init_kvengine();

#if (ENABLE_NETWORK_SELECT == NETWORK_EPOLL)
    epoll_entry();
#elif (ENABLE_NETWORK_SELECT == NETWORK_WAVECO)
    waveco_entry();
#elif (ENABLE_NETWORK_SELECT == NETWORK_IOURING)
    
#endif

    exit_kvengine();
}
