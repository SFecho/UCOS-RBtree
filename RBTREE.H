#ifndef _RBTREE_H
#define _RBTREE_H

#include "UCOS.H"
#include <conio.h>
//#define		RED		1
//#define		BLACK		0

//typedef struct _rb_node rb_node;

typedef struct _rb_tree rb_tree;

struct _rb_tree
{
	OS_TCB * root;
	OS_TCB * nil;

	/**
	 * first < *second <--> return -1
     * first > *second <--> return 1
     * first = *second <--> return 1
     *
     */
	int	(*rb_insert)(rb_tree * tree, OS_TCB *node);
	OS_TCB *	(*get_node)(rb_tree *tree, UBYTE prio);
	OS_TCB * ( *rb_delete)(rb_tree * tree, UBYTE prio);
	OS_TCB * (*get_min_node)(rb_tree * tree);
};

rb_tree * get_rb_tree();
#endif