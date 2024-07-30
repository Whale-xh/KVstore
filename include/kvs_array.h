#ifndef __KVS_ARRAY_H__
#define __KVS_ARRAY_H__

#define KVS_ARRAY_SIZE 1024

struct kvs_array_item
{
    char *key;
    char *value;
};
typedef struct array_s
{
    struct kvs_array_item *array_tables;
    int array_idx;
} array_t;

extern array_t kvs_Array;
int kvstore_array_create(array_t *arr);
void kvstore_array_destory(array_t *arr);
int kvstore_array_set(char *key, char *value);
char *kvstore_array_get(char *key);
int kvstore_array_delete(char *key);
int kvstore_array_modify(char *key, char *value);
int kvstore_array_count();
int array_set(array_t *arr,char *key, char *value);
char *array_get(array_t *arr,char *key);
int array_del(array_t *arr,char *key);
int array_modify(array_t *arr,char *key, char *value);
int array_count(array_t *arr);


#endif


