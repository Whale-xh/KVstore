#include "../include/kvs_rbtree.h"
#include "../include/kvstore.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

rbtree_t kvs_Tree;

rbtree_node *rbtree_mini(rbtree *T, rbtree_node *x)
{
	while (x->left != T->nil)
	{
		x = x->left;
	}
	return x;
}

rbtree_node *rbtree_maxi(rbtree *T, rbtree_node *x)
{
	while (x->right != T->nil)
	{
		x = x->right;
	}
	return x;
}

rbtree_node *rbtree_successor(rbtree *T, rbtree_node *x)
{
	rbtree_node *y = x->parent;

	if (x->right != T->nil)
	{
		return rbtree_mini(T, x->right);
	}

	while ((y != T->nil) && (x == y->right))
	{
		x = y;
		y = y->parent;
	}
	return y;
}

void rbtree_left_rotate(rbtree *T, rbtree_node *x)
{

	rbtree_node *y = x->right; // x  --> y  ,  y --> x,   right --> left,  left --> right

	x->right = y->left; // 1 1
	if (y->left != T->nil)
	{ // 1 2
		y->left->parent = x;
	}

	y->parent = x->parent; // 1 3
	if (x->parent == T->nil)
	{ // 1 4
		T->root = y;
	}
	else if (x == x->parent->left)
	{
		x->parent->left = y;
	}
	else
	{
		x->parent->right = y;
	}

	y->left = x;   // 1 5
	x->parent = y; // 1 6
}

void rbtree_right_rotate(rbtree *T, rbtree_node *y)
{

	rbtree_node *x = y->left;

	y->left = x->right;
	if (x->right != T->nil)
	{
		x->right->parent = y;
	}

	x->parent = y->parent;
	if (y->parent == T->nil)
	{
		T->root = x;
	}
	else if (y == y->parent->right)
	{
		y->parent->right = x;
	}
	else
	{
		y->parent->left = x;
	}

	x->right = y;
	y->parent = x;
}

void rbtree_insert_fixup(rbtree *T, rbtree_node *z)
{

	while (z->parent->color == RED)
	{ // z ---> RED
		if (z->parent == z->parent->parent->left)
		{
			rbtree_node *y = z->parent->parent->right;
			if (y->color == RED)
			{
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;

				z = z->parent->parent; // z --> RED
			}
			else
			{

				if (z == z->parent->right)
				{
					z = z->parent;
					rbtree_left_rotate(T, z);
				}

				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rbtree_right_rotate(T, z->parent->parent);
			}
		}
		else
		{
			rbtree_node *y = z->parent->parent->left;
			if (y->color == RED)
			{
				z->parent->color = BLACK;
				y->color = BLACK;
				z->parent->parent->color = RED;

				z = z->parent->parent; // z --> RED
			}
			else
			{
				if (z == z->parent->left)
				{
					z = z->parent;
					rbtree_right_rotate(T, z);
				}

				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rbtree_left_rotate(T, z->parent->parent);
			}
		}
	}

	T->root->color = BLACK;
}

void rbtree_insert(rbtree *T, rbtree_node *z)
{

	rbtree_node *y = T->nil;
	rbtree_node *x = T->root;

	while (x != T->nil)
	{
		y = x;
#if ENABLE_KEY_CHAR
		if (strcmp(z->key, x->key) < 0)
		{
			x = x->left;
		}
		else if (strcmp(z->key, x->key) > 0)
		{
			x = x->right;
		}
		else
		{
			return;
		}
#else
		if (z->key < x->key)
		{
			x = x->left;
		}
		else if (z->key > x->key)
		{
			x = x->right;
		}
		else
		{ // Exist
			return;
		}
#endif
	}

	z->parent = y;
	if (y == T->nil)
	{
		T->root = z;
	}
#if ENABLE_KEY_CHAR
	else if (strcmp(z->key, y->key) < 0)
#else
	else if (z->key < y->key)
#endif
	{
		y->left = z;
	}
	else
	{
		y->right = z;
	}

	z->left = T->nil;
	z->right = T->nil;
	z->color = RED;

	rbtree_insert_fixup(T, z);
}

void rbtree_delete_fixup(rbtree *T, rbtree_node *x)
{

	while ((x != T->root) && (x->color == BLACK))
	{
		if (x == x->parent->left)
		{

			rbtree_node *w = x->parent->right;
			if (w->color == RED)
			{
				w->color = BLACK;
				x->parent->color = RED;

				rbtree_left_rotate(T, x->parent);
				w = x->parent->right;
			}

			if ((w->left->color == BLACK) && (w->right->color == BLACK))
			{
				w->color = RED;
				x = x->parent;
			}
			else
			{

				if (w->right->color == BLACK)
				{
					w->left->color = BLACK;
					w->color = RED;
					rbtree_right_rotate(T, w);
					w = x->parent->right;
				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->right->color = BLACK;
				rbtree_left_rotate(T, x->parent);

				x = T->root;
			}
		}
		else
		{

			rbtree_node *w = x->parent->left;
			if (w->color == RED)
			{
				w->color = BLACK;
				x->parent->color = RED;
				rbtree_right_rotate(T, x->parent);
				w = x->parent->left;
			}

			if ((w->left->color == BLACK) && (w->right->color == BLACK))
			{
				w->color = RED;
				x = x->parent;
			}
			else
			{

				if (w->left->color == BLACK)
				{
					w->right->color = BLACK;
					w->color = RED;
					rbtree_left_rotate(T, w);
					w = x->parent->left;
				}

				w->color = x->parent->color;
				x->parent->color = BLACK;
				w->left->color = BLACK;
				rbtree_right_rotate(T, x->parent);

				x = T->root;
			}
		}
	}

	x->color = BLACK;
}

rbtree_node *rbtree_delete(rbtree *T, rbtree_node *z)
{

	rbtree_node *y = T->nil;
	rbtree_node *x = T->nil;

	if ((z->left == T->nil) || (z->right == T->nil))
	{
		y = z;
	}
	else
	{
		y = rbtree_successor(T, z);
	}

	if (y->left != T->nil)
	{
		x = y->left;
	}
	else if (y->right != T->nil)
	{
		x = y->right;
	}

	x->parent = y->parent;
	if (y->parent == T->nil)
	{
		T->root = x;
	}
	else if (y == y->parent->left)
	{
		y->parent->left = x;
	}
	else
	{
		y->parent->right = x;
	}

	if (y != z)
	{
#if ENABLE_KEY_CHAR
		void *tmp = z->key;
		z->key = y->key;
		y->key = tmp;

		tmp = z->value;
		z->value = y->value;
		y->value = tmp;
#else
		z->key = y->key;
		z->value = y->value;
#endif
	}

	if (y->color == BLACK)
	{
		rbtree_delete_fixup(T, x);
	}

	return y;
}

rbtree_node *rbtree_search(rbtree *T, KEY_TYPE key)
{

	rbtree_node *node = T->root;
	while (node != T->nil)
	{
#if ENABLE_KEY_CHAR
		if (strcmp(key, node->key) < 0)
		{
			node = node->left;
		}
		else if (strcmp(key, node->key) > 0)
		{
			node = node->right;
		}
		else
		{
			return node;
		}
#else
		if (key < node->key)
		{
			node = node->left;
		}
		else if (key > node->key)
		{
			node = node->right;
		}
		else
		{
			return node;
		}
#endif
	}
	return T->nil;
}

void rbtree_traversal(rbtree *T, rbtree_node *node)
{
	if (node != T->nil)
	{
		rbtree_traversal(T, node->left);
#if ENABLE_KEY_CHAR
		printf("key:%s, color:%d\n", node->key, node->color);
#else
		printf("key:%d, color:%d\n", node->key, node->color);
#endif
		rbtree_traversal(T, node->right);
	}
}

int kvstore_rbtree_create(rbtree_t *tree)
{
	if (!tree)
		return -1;
	memset(tree,0,sizeof(tree));
	tree->nil = (rbtree_node *)kvstore_malloc(sizeof(rbtree_node));
	tree->nil->key = kvstore_malloc(1);
	*(tree->nil->key) = '\0';
	tree->nil->color = BLACK;
	tree->root = tree->nil;
	return 0;
}

void kvstore_rbtree_destory(rbtree_t *tree)
{
	if (!tree)
		return;
	if (!tree->nil->key)
		kvstore_free(tree->nil->key);
	rbtree_node *node = tree->root;
	while (node != tree->nil)
	{
		node = rbtree_mini(tree, tree->root);
		if (node == tree->nil)
		{
			break;
		}
		node = rbtree_delete(tree, node);
		if (!node)
		{
			kvstore_free(node->key);
			kvstore_free(node->value);
			kvstore_free(node);
		}
	}
	tree->count=0;
}

int rbtree_set(rbtree_t *tree, char *key, char *value)
{
	rbtree_node *node = (rbtree_node *)kvstore_malloc(sizeof(rbtree_node));
	if (!node)
		return -1;
	node->key = kvstore_malloc(strlen(key) + 1);
	if (!node->key)
	{
		kvstore_free(node);
		return -1;
	}
	memset(node->key, 0, strlen(key) + 1);
	strcpy(node->key, key);
	node->value = kvstore_malloc(strlen(value) + 1);
	if (!node->value)
	{
		kvstore_free(node);
		kvstore_free(node->key);
		return -1;
	}
	memset(node->value, 0, strlen(value) + 1);
	strcpy((char *)node->value, value);
	rbtree_insert(tree,node);
	++(tree->count);
	return 0;
}

char *rbtree_get(rbtree_t *tree, char *key)
{
	rbtree_node *node = rbtree_search(tree, key);
	if (node == tree->nil)
	{
		return NULL;
	}
	return node->value;
}

int rbtree_del(rbtree_t *tree, char *key)
{
	rbtree_node *node = rbtree_search(tree, key);
	if (node == tree->nil)
	{
		return -1;
	}
	rbtree_node *cur = rbtree_delete(tree, node);
	if (!cur)
	{
		kvstore_free(cur->key);
		kvstore_free(cur->value);
		kvstore_free(cur);
	}
	--(tree->count);
	return 0;
}

int rbtree_modify(rbtree_t *tree, char *key, char *value)
{
	rbtree_node *node = rbtree_search(tree, key);
	if (node == tree->nil)
	{
		return -1;
	}
	char *tmp = node->value;
	kvstore_free(tmp);
	node->value = kvstore_malloc(strlen(value) + 1);
	if (!node->value)
	{
		return -1;
	}
	strcpy(node->value, value);
	return 0;
}

int rbtree_count(rbtree_t *tree)
{
	return tree->count;
}

