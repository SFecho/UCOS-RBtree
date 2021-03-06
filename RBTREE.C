
//#include "UCOS.H"
#include <string.h>
#include <stdio.h>
#include "RBTree.h"
#include "UCOS186C.H"
#include "UCOS.H"

extern rb_tree  tree_manager;


static void left_rotate(rb_tree * tree, OS_TCB * node)
{
	
	OS_TCB * rotate_node = node->OSTCBNext;	//该节点的右儿子
	node->OSTCBNext = rotate_node->OSTCBPrev;
	
	if(node == tree->nil )
		return ;
	
	if (rotate_node->OSTCBPrev != tree->nil)
		rotate_node->OSTCBPrev->OSTCBParent = node;
	
	rotate_node->OSTCBPrev = node;
	
	if (node->OSTCBParent == tree->nil)
		tree->root = rotate_node;
	else
	{
		if (node->OSTCBParent->OSTCBPrev == node)
			node->OSTCBParent->OSTCBPrev = rotate_node;
		else
			node->OSTCBParent->OSTCBNext = rotate_node;
	}
	rotate_node->OSTCBParent = node->OSTCBParent;
	
	node->OSTCBParent = rotate_node;
	
}

/**
 *右旋函数
 *
 *
 */
static void right_rotate(rb_tree * tree, OS_TCB * node)
{
	OS_TCB * rotate_node = node->OSTCBPrev;				//该节点的左儿子
	node->OSTCBPrev = rotate_node->OSTCBNext;
	
	if(node == tree->nil )
		return ;
	
	if (rotate_node->OSTCBNext != tree->nil)
		rotate_node->OSTCBNext->OSTCBParent = node;
	
	rotate_node->OSTCBNext = node;
	
	if( node->OSTCBParent == tree->nil )
		tree->root = rotate_node;
	else
	{
		if (node->OSTCBParent->OSTCBPrev == node)
			node->OSTCBParent->OSTCBPrev = rotate_node;
		else
			node->OSTCBParent->OSTCBNext = rotate_node;
	}
	
	rotate_node->OSTCBParent = node->OSTCBParent;
	
	node->OSTCBParent = rotate_node;
}
/**
 *
 * 插入后修复函数，使得新的树满足红黑树的性质
 *
 */
static void insert_fix_up(rb_tree * tree, OS_TCB * node)
{
	OS_TCB * uncle = tree->nil;
	while (node->OSTCBParent->color == RED)						//判断父亲节点是否为红色
	{
		if (node->OSTCBParent->OSTCBParent->OSTCBPrev == node->OSTCBParent)		//如果父亲节点是爷爷节点的左儿子
		{
			uncle = node->OSTCBParent->OSTCBParent->OSTCBNext;				//得到叔叔节点的位置
			if (uncle->color == RED)							//case 1:如果叔叔节点为红色，
			{
				uncle->color = node->OSTCBParent->color = BLACK;		//case 1:将父亲节点与叔叔节点涂成黑色
				uncle->OSTCBParent->color = RED;						//case 1:将爷爷节点涂成红色，这样能够保证黑高不变
				node = uncle->OSTCBParent;							//case 1:将当前节点调整至爷爷节点
			}
			else
			{
				if (node->OSTCBParent->OSTCBNext == node)				//case 2:如果当前节点是父亲节点的右儿子，即爷爷节点，父亲节点，与当前节点不在同一侧
				{
					node = node->OSTCBParent;						//case 2:将当前节点调整至其父亲节点
					left_rotate(tree, node);					//case 2:对当前节点进行左旋
				}

				node->OSTCBParent->color = BLACK;					//case 3:如果当前节点是父亲节点的左儿子，即爷爷节点，父亲节点，与当前节点在同一侧
				node->OSTCBParent->OSTCBParent->color = RED;				//case 3:将父亲节点涂成黑色，爷爷节点涂成红色，以便于尽可能保证黑高一定又不违反红黑树的性质
				right_rotate(tree, node->OSTCBParent->OSTCBParent);		//case 3:对其爷爷节点进行右旋
			}
		}
		else if (node->OSTCBParent->OSTCBParent->OSTCBNext == node->OSTCBParent)	//如果父亲节点是爷爷节点的右儿子	
		{
			uncle = node->OSTCBParent->OSTCBParent->OSTCBPrev;					//得到叔叔节点的位置
			if (uncle->color == RED)							//case 1:如果叔叔节点为红色，
			{
				uncle->color = node->OSTCBParent->color = BLACK;		//case 1:将父亲节点与叔叔节点涂成黑色
				uncle->OSTCBParent->color = RED;						//case 1:将爷爷节点涂成红色，这样能够保证黑高不变
				node = uncle->OSTCBParent;							//case 1:将当前节点调整至爷爷节点
			}
			else
			{
				if (node->OSTCBParent->OSTCBPrev == node)				//case 2:如果当前节点是父亲节点的左儿子，即爷爷节点，父亲节点，与当前节点不在同一侧
				{
					node = node->OSTCBParent;						//case 2:将当前节点调整至其父亲节点
					right_rotate(tree, node);					//case 2:对当前节点进行右旋
				}
				node->OSTCBParent->color = BLACK;					//case 3:如果当前节点是父亲节点的右儿子，即爷爷节点，父亲节点，与当前节点在同一侧
				node->OSTCBParent->OSTCBParent->color = RED;				//case 3:将父亲节点涂成黑色，爷爷节点涂成红色，以便于尽可能保证黑高一定又不违反红黑树的性质
				left_rotate(tree, node->OSTCBParent->OSTCBParent);		//case 3:对其爷爷节点进行左旋
			}
		}
		
	}
	tree->nil->color = BLACK;
	tree->root->color = BLACK;									//将根节点涂成黑色，以便于满足红黑树的性质
}
int rb_insert(rb_tree * tree, OS_TCB *node)
{
	OS_TCB *itr = tree->root;

	OS_TCB *prev = tree->nil;

	while (itr != tree->nil)
	{
		prev = itr;

		if (node->OSTCBPrio< itr->OSTCBPrio)
			itr = itr->OSTCBPrev;
		else if (node->OSTCBPrio == itr->OSTCBPrio)
		{
			return 0;
		}
		else
			itr = itr->OSTCBNext;
	}

	if (prev == tree->nil)
		tree->root = node;
	else

	{
		if (node->OSTCBPrio <  prev->OSTCBPrio)
			prev->OSTCBPrev = node;
		else
			prev->OSTCBNext = node;
	}

	node->OSTCBPrev = node->OSTCBNext = tree->nil;
	node->OSTCBParent = prev;
	node->color = RED;

	insert_fix_up(tree, node);

	return 1;

}


static void transplant(rb_tree *tree, OS_TCB * delete_node, OS_TCB * next_node)
{
	if ( delete_node == tree->nil)
		return ;
	if (delete_node->OSTCBParent != tree->nil)				//如果待删除的节点的父亲节点不为空，即该节点不为根节点
	{
		if (delete_node->OSTCBParent->OSTCBPrev == delete_node)	//如果待删节点是其父亲节点的左儿子
			delete_node->OSTCBParent->OSTCBPrev = next_node;		//将其父亲节点的左指针连接至next_node
		else
			delete_node->OSTCBParent->OSTCBNext = next_node;		//将其父亲节点的右指针连接至next_node
	}
	else
		tree->root = next_node;							//如果根节点为待删除节点，则直接将根节点指向next_node
	//添加了if
	if(next_node != tree->nil)
		next_node->OSTCBParent = delete_node->OSTCBParent;			//将新节点的父亲指针指向待删除的节点的父亲指针指向的位置
}


//修复删除后的红黑树
static void delete_fix_up(rb_tree * tree, OS_TCB * node)
{
	OS_TCB * brothers = tree->nil;
	while (node != tree->root && node->color == BLACK)
	{
		if (node == node->OSTCBParent->OSTCBPrev)						//当前节点是其父亲的左儿子
		{
			brothers = node->OSTCBParent->OSTCBNext;
								
			if (RED == brothers->color)						//case 1： node的兄弟是红色的，通过
			{
				brothers->color = BLACK;					//case 1：将兄弟节点置黑
				brothers->OSTCBParent->color = RED;				//case 1：父亲节点置红
				left_rotate(tree, node->OSTCBParent);			//case 1：以父亲节点进行左旋
				brothers = node->OSTCBParent->OSTCBNext;				//case 1：重置兄弟节点位置
			}												//case 1：处理完情况1之后，w.color== BLACK ， 情况就变成2 3 4 了
																		
			if (brothers->OSTCBPrev->color == BLACK && brothers->OSTCBNext->color == BLACK)		//case 2： node的兄弟是黑色的，并且其儿子都是黑色的。
			{
				if (node->OSTCBParent->color == RED)				//case 2：如果父亲节点为红色
				{
					node->OSTCBParent->color = BLACK;			//case 2：将父亲节点置黑
					brothers->color = RED;					//case 2：兄弟节点置红
					break;									//case 2：退出循环
				}
				else
				{
					brothers->color = RED;					//case 2：兄弟节点置红，以父亲节点为新的基准点进行矫正									
					node = node->OSTCBParent;					//case 2：x.p左右是平衡的，但是x.p处少了一个黑结点，所以把x.p作为新的x继续循环
					continue;								//case 2：进行下一次循环
				}
			}

			
			if (brothers->OSTCBNext->color == BLACK)			//case 3：兄弟节点为黑色的，左孩子为红色。（走到这一步，说明w左右不同时为黑色。）
			{
				brothers->OSTCBPrev->color = BLACK;				//case 3：让兄弟节点的左儿子置红
				brothers->color = RED;						//case 3：兄弟节点置红
				right_rotate(tree, brothers);				//case 3：右旋
				brothers = node->OSTCBParent->OSTCBNext;				//case 3：兄弟节点重置
			}

			

			brothers->color = node->OSTCBParent->color;			//case 4： 走到这一步说明兄弟节点为黑色， 兄弟节点的左孩子为黑色， 右孩子为红色。
			node->OSTCBParent->color = BLACK;					//case 4： 将父亲节点置黑
			brothers->OSTCBParent->color = BLACK;					//case 4： 兄弟的右儿子节点置黑
			left_rotate(tree, node->OSTCBParent);				//case 4： 以父亲节点进行左旋
			node = tree->root;								//case 4： 将node置为根节点
		}

		else{												//当前节点是其父亲的右儿子
			brothers = node->OSTCBParent->OSTCBPrev;					//找到兄弟节点的位置
			//1
			if (brothers->color == RED)						//case 1：如果兄弟节点为红色
			{
				brothers->color = BLACK;					//case 1：将其兄弟节点置为黑色
				node->OSTCBParent->color = RED;					//case 1：将其父亲节点置为红色
				right_rotate(tree, node->OSTCBParent);			//case 1：对其父亲节点进行右旋
				brothers = node->OSTCBParent->OSTCBPrev;				//case 1：重置兄弟节点位置
			}
			//2
			if (brothers->OSTCBPrev->color == BLACK && brothers->OSTCBPrev->color == BLACK)	//case 2： node的兄弟是黑色的，并且其儿子都是黑色的。
			{
				if (node->OSTCBParent->color == RED)				//case 2：如果其父亲节点为红色
				{													
					node->OSTCBParent->color = BLACK;			//case 2：将其父亲节点置为黑色
					brothers->color = RED;					//case 2：其兄弟节点置为红色
					break;									//case 2：跳出循环
				}
				else										//case 2：其父亲节点为黑色
				{
					node->OSTCBParent->color = BLACK;			//case 2：将其父亲节点染成黑色
					brothers->color = RED;					//case 2：将其兄弟节点置为红色
					node = node->OSTCBParent;					//case 2：将当前结点调至父亲位置
					continue;								//进入下一次循环
				}
			}

			//3
			if (brothers->OSTCBPrev->color == BLACK)				//case 3：如果兄弟左儿子为黑色
			{
				brothers->color = RED;						//case 3：将兄弟节点置红
				brothers->OSTCBNext->color = BLACK;				//case 3：右儿子置黑
				brothers = node->OSTCBParent->OSTCBPrev;				//case 3：重置兄弟节点
			}

			//4
			brothers->color = brothers->OSTCBParent->color;		//case 4：将兄弟颜色置为其父亲的颜色
			node->OSTCBParent->color = BLACK;					//case 4：将其父亲节点置黑
			brothers->OSTCBPrev->color = BLACK;					//case 4：兄弟的左儿子置黑
			right_rotate(tree, node->OSTCBParent);				//case 4：对其父亲节点进行右旋
			node = tree->root;								//case 4：将node置为根节点
		}
	}
	node->color = BLACK;									//将当前结点置为黑色
	tree->nil->OSTCBParent = tree->nil;
	tree->nil->color = BLACK;
}

OS_TCB * get_node(rb_tree * tree, UBYTE prio)
{
	OS_TCB * node = tree->root;
	while(node != tree->nil)
	{
		if(node->OSTCBPrio == prio)
			return node;
		else if(node->OSTCBPrio > prio)
			node = node->OSTCBPrev;
		else
			node = node->OSTCBNext;
	}
	return NULL;
}

//删除红黑树
OS_TCB * rb_delete(rb_tree * tree, UBYTE prio)
{
	OS_TCB * delete_node = NULL;
	OS_TCB *min_node = NULL;
	OS_TCB * fix_node = NULL;
	//OS_TCB	tmp_node;
      //	OS_TCB * tmp = NULL;
	int color;
	if (tree == NULL || tree->root == tree->nil)
		return NULL;

	delete_node = get_node(tree, prio);								// 待删除节点


	if (delete_node == tree->nil)
		return NULL;
	color = delete_node->color;

	min_node = delete_node;
	fix_node = tree->nil;
	if (delete_node->OSTCBPrev == tree->nil)
	{
		fix_node = delete_node->OSTCBNext;
		transplant(tree, delete_node, delete_node->OSTCBNext);
	}
	else if (delete_node->OSTCBNext == tree->nil)
	{
		fix_node = delete_node->OSTCBPrev;
		transplant(tree, delete_node, delete_node->OSTCBPrev);
	}
	else
	{
		min_node = delete_node->OSTCBNext;
		while (min_node != tree->nil && min_node->OSTCBPrev != tree->nil)
			min_node = min_node->OSTCBPrev;

		min_node->color = color;
		fix_node = min_node->OSTCBNext;

		if (min_node->OSTCBParent == delete_node)
			fix_node->OSTCBParent = min_node;
		else{
			transplant(tree, min_node, min_node->OSTCBNext);
			min_node->OSTCBNext = delete_node->OSTCBNext;
			min_node->OSTCBNext->OSTCBParent = min_node;
		}
		transplant(tree, delete_node, min_node);
		min_node->OSTCBPrev = delete_node->OSTCBPrev;
		min_node->OSTCBPrev->OSTCBParent = min_node;
		min_node->color = delete_node->color;
	}
	if (color == BLACK)
		delete_fix_up(tree, fix_node);

	return delete_node;
}

OS_TCB * get_min_node(rb_tree * tree)
{
	OS_TCB * itr = tree->root;
	while (itr->OSTCBPrev != tree->nil)
		itr = itr->OSTCBPrev;
	return itr;
//get_min_node(rb_tree * tree)
}

//初始化红黑树
rb_tree * get_rb_tree()
{
	rb_tree * _tree = &tree_manager;
	OS_TCB *node = OSTCBGetFree();
	
	node->color = BLACK;
	node->OSTCBPrev = node->OSTCBNext = node->OSTCBParent = node;
	
	_tree->root = _tree->nil = node;
		
	_tree->rb_insert = rb_insert;

	_tree->get_node = get_node;

	_tree->rb_delete = rb_delete;

	_tree->get_min_node = get_min_node;

	return _tree;
}


