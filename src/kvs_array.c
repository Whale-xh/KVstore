#include "../include/kvs_array.h"
#include "../include/kvstore.h"
#include <string.h>

array_t kvs_Array;

int kvstore_array_create(array_t *arr)
{
    if (!arr)
        return -1;
    arr->array_tables = kvstore_malloc(KVS_ARRAY_SIZE * sizeof(struct kvs_array_item));
    if (!arr->array_tables)
    {
        return -1;
    }
    memset(arr->array_tables, 0, KVS_ARRAY_SIZE * sizeof(struct kvs_array_item));
    arr->array_idx = 0;
    return 0;
}

void kvstore_array_destory(array_t *arr)
{
    if (!arr)
        return;
    if (arr->array_tables)
        kvstore_free(arr->array_tables);
    arr->array_idx = 0;
}

int array_set(array_t *arr,char *key, char *value)
{
    if (!arr || key == NULL || value == NULL || arr->array_idx == KVS_ARRAY_SIZE)
        return -1;
    char *kcopy = kvstore_malloc(strlen(key) + 1);
    if (kcopy == NULL)
        return -1;
    strncpy(kcopy, key, strlen(key) + 1);
    char *vcopy = kvstore_malloc(strlen(value) + 1);
    if (vcopy == NULL)
    {
        kvstore_free(kcopy);
        return -1;
    }
    strncpy(vcopy, value, strlen(value) + 1);

    int i = 0;
    for (i = 0; i < arr->array_idx; ++i)
    {
        if (arr->array_tables[i].key == NULL)
        {
           arr->array_tables[i].key = kcopy;
            arr->array_tables[i].value = vcopy;
        }
    }
    if (i < KVS_ARRAY_SIZE && i == arr->array_idx)
    {
        arr->array_tables[arr->array_idx].key = kcopy;
        arr->array_tables[arr->array_idx].value = vcopy;
    }
    ++(arr->array_idx);
    return 0;
}

char *array_get(array_t *arr,char *key)
{
    if(!arr) return NULL;
    for (int i = 0; i < arr->array_idx; ++i)
    {
        if (arr->array_tables[i].key == NULL)
            return NULL;
        if (strcmp(arr->array_tables[i].key, key) == 0)
        {
            return arr->array_tables[i].value;
        }
    }
    return NULL;
}

// return > 0 ,this key not exist
int array_del(array_t *arr,char *key)
{
    int i = 0;
    if (!arr || key == NULL)
        return -1;
    for (i = 0; i < arr->array_idx; ++i)
    {
        if (strcmp(arr->array_tables[i].key, key) == 0)
        {
            kvstore_free(arr->array_tables[i].key);
            arr->array_tables[i].key = NULL;
            kvstore_free(arr->array_tables[i].value);
            arr->array_tables[i].value = NULL;
            --(arr->array_idx);
            return 0;
        }
    }
    return i;
}

int array_modify(array_t *arr,char *key, char *value)
{
    int i = 0;
    if (!arr || key == NULL || value == NULL)
        return -1;
    for (i = 0; i < arr->array_idx; ++i)
    {
        if (strcmp(arr->array_tables[i].key, key) == 0)
        {
            memset(arr->array_tables[i].value, 0, strlen(arr->array_tables[i].value) + 1);
            strncpy(arr->array_tables[i].value, value, strlen(value) + 1);
            return 0;
        }
    }
    return i;
}

int array_count(array_t *arr)
{
    if(!arr) return -1;
    return arr->array_idx;
}