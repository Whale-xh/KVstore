#ifndef __KVS_HASH_H__
#define __KVS_HASH_H__

#define MAX_KEY_LEN_HASH	128
#define ENABLE_POINTER_KEY	1    // 1: Structure storing strings  

typedef struct hashnode_s {
#if ENABLE_POINTER_KEY
	char *key;
	char *value;
#else
	char key[MAX_KEY_LEN_HASH];
	char value[MAX_VALUE_LEN_HASH];
#endif	
	struct hashnode_s *next;
	
} hashnode_t;


typedef struct hashtable_s {
	hashnode_t **nodes;
	int max_slots;
	int count;
	//pthread_mutex_t lock;

} hashtable_t;

extern hashtable_t kvs_Hash;

int kvstore_hash_create(hashtable_t *hash);
void kvstore_hash_destory(hashtable_t *hash);
int kvstore_hash_set(char *key, char *value);
char *kvstore_hash_get(char *key);
int kvstore_hash_delete(char *key);
int kvstore_hash_modify(char *key, char *value);
int kvstore_hash_count();
int hash_set(hashtable_t *hash, char *key, char *value);
char *hash_get(hashtable_t *hash, char *key);
int hash_delete(hashtable_t *hash, char *key);
int hash_modify(hashtable_t *hash, char *key, char *value);
int hash_count(hashtable_t *hash);


#endif