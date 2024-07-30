#ifndef __KVS_RBTREE_H__
#define __KVS_RBTREE_H__

#define RED 1
#define BLACK 2

#define ENABLE_KEY_CHAR 1
#define MAX_KEY_LEN 256
#define MAX_VALUUE_LEN 1024

#if ENABLE_KEY_CHAR
typedef char *KEY_TYPE;
#else
typedef int KEY_TYPE;
#endif

typedef struct _rbtree_node
{
	unsigned char color;
	struct _rbtree_node *right;
	struct _rbtree_node *left;
	struct _rbtree_node *parent;
	KEY_TYPE key;
	void *value;
} rbtree_node;

typedef struct _rbtree
{
	rbtree_node *root;
	rbtree_node *nil;
	int count;
} rbtree;
typedef struct _rbtree rbtree_t;
extern rbtree_t kvs_Tree;
int kvstore_rbtree_create(rbtree_t *tree);
void kvstore_rbtree_destory(rbtree_t *tree);
int kvstore_rbtree_set(char *key, char *value);
char *kvstore_rbtree_get(char *key);
int kvstore_rbtree_delete(char *key);
int kvstore_rbtree_modify(char *key, char *value);
int kvstore_rbtree_count();
int rbtree_set(rbtree_t *tree, char *key, char *value);
char *rbtree_get(rbtree_t *tree, char *key);
int rbtree_del(rbtree_t *tree, char *key);
int rbtree_modify(rbtree_t *tree, char *key, char *value);
int rbtree_count(rbtree_t *tree);



#endif