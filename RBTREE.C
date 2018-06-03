
//#include "UCOS.H"
#include <string.h>
#include <stdio.h>
#include "RBTree.h"
#include "UCOS186C.H"
#include "UCOS.H"

extern rb_tree  tree_manager;


static void left_rotate(rb_tree * tree, OS_TCB * node)
{
	
	OS_TCB * rotate_node = node->OSTCBNext;	//�ýڵ���Ҷ���
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
 *��������
 *
 *
 */
static void right_rotate(rb_tree * tree, OS_TCB * node)
{
	OS_TCB * rotate_node = node->OSTCBPrev;				//�ýڵ�������
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
 * ������޸�������ʹ���µ�����������������
 *
 */
static void insert_fix_up(rb_tree * tree, OS_TCB * node)
{
	OS_TCB * uncle = tree->nil;
	while (node->OSTCBParent->color == RED)						//�жϸ��׽ڵ��Ƿ�Ϊ��ɫ
	{
		if (node->OSTCBParent->OSTCBParent->OSTCBPrev == node->OSTCBParent)		//������׽ڵ���үү�ڵ�������
		{
			uncle = node->OSTCBParent->OSTCBParent->OSTCBNext;				//�õ�����ڵ��λ��
			if (uncle->color == RED)							//case 1:�������ڵ�Ϊ��ɫ��
			{
				uncle->color = node->OSTCBParent->color = BLACK;		//case 1:�����׽ڵ�������ڵ�Ϳ�ɺ�ɫ
				uncle->OSTCBParent->color = RED;						//case 1:��үү�ڵ�Ϳ�ɺ�ɫ�������ܹ���֤�ڸ߲���
				node = uncle->OSTCBParent;							//case 1:����ǰ�ڵ������үү�ڵ�
			}
			else
			{
				if (node->OSTCBParent->OSTCBNext == node)				//case 2:�����ǰ�ڵ��Ǹ��׽ڵ���Ҷ��ӣ���үү�ڵ㣬���׽ڵ㣬�뵱ǰ�ڵ㲻��ͬһ��
				{
					node = node->OSTCBParent;						//case 2:����ǰ�ڵ�������丸�׽ڵ�
					left_rotate(tree, node);					//case 2:�Ե�ǰ�ڵ��������
				}

				node->OSTCBParent->color = BLACK;					//case 3:�����ǰ�ڵ��Ǹ��׽ڵ������ӣ���үү�ڵ㣬���׽ڵ㣬�뵱ǰ�ڵ���ͬһ��
				node->OSTCBParent->OSTCBParent->color = RED;				//case 3:�����׽ڵ�Ϳ�ɺ�ɫ��үү�ڵ�Ϳ�ɺ�ɫ���Ա��ھ����ܱ�֤�ڸ�һ���ֲ�Υ�������������
				right_rotate(tree, node->OSTCBParent->OSTCBParent);		//case 3:����үү�ڵ��������
			}
		}
		else if (node->OSTCBParent->OSTCBParent->OSTCBNext == node->OSTCBParent)	//������׽ڵ���үү�ڵ���Ҷ���	
		{
			uncle = node->OSTCBParent->OSTCBParent->OSTCBPrev;					//�õ�����ڵ��λ��
			if (uncle->color == RED)							//case 1:�������ڵ�Ϊ��ɫ��
			{
				uncle->color = node->OSTCBParent->color = BLACK;		//case 1:�����׽ڵ�������ڵ�Ϳ�ɺ�ɫ
				uncle->OSTCBParent->color = RED;						//case 1:��үү�ڵ�Ϳ�ɺ�ɫ�������ܹ���֤�ڸ߲���
				node = uncle->OSTCBParent;							//case 1:����ǰ�ڵ������үү�ڵ�
			}
			else
			{
				if (node->OSTCBParent->OSTCBPrev == node)				//case 2:�����ǰ�ڵ��Ǹ��׽ڵ������ӣ���үү�ڵ㣬���׽ڵ㣬�뵱ǰ�ڵ㲻��ͬһ��
				{
					node = node->OSTCBParent;						//case 2:����ǰ�ڵ�������丸�׽ڵ�
					right_rotate(tree, node);					//case 2:�Ե�ǰ�ڵ��������
				}
				node->OSTCBParent->color = BLACK;					//case 3:�����ǰ�ڵ��Ǹ��׽ڵ���Ҷ��ӣ���үү�ڵ㣬���׽ڵ㣬�뵱ǰ�ڵ���ͬһ��
				node->OSTCBParent->OSTCBParent->color = RED;				//case 3:�����׽ڵ�Ϳ�ɺ�ɫ��үү�ڵ�Ϳ�ɺ�ɫ���Ա��ھ����ܱ�֤�ڸ�һ���ֲ�Υ�������������
				left_rotate(tree, node->OSTCBParent->OSTCBParent);		//case 3:����үү�ڵ��������
			}
		}
		
	}
	tree->nil->color = BLACK;
	tree->root->color = BLACK;									//�����ڵ�Ϳ�ɺ�ɫ���Ա�����������������
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
	if (delete_node->OSTCBParent != tree->nil)				//�����ɾ���Ľڵ�ĸ��׽ڵ㲻Ϊ�գ����ýڵ㲻Ϊ���ڵ�
	{
		if (delete_node->OSTCBParent->OSTCBPrev == delete_node)	//�����ɾ�ڵ����丸�׽ڵ�������
			delete_node->OSTCBParent->OSTCBPrev = next_node;		//���丸�׽ڵ����ָ��������next_node
		else
			delete_node->OSTCBParent->OSTCBNext = next_node;		//���丸�׽ڵ����ָ��������next_node
	}
	else
		tree->root = next_node;							//������ڵ�Ϊ��ɾ���ڵ㣬��ֱ�ӽ����ڵ�ָ��next_node
	//������if
	if(next_node != tree->nil)
		next_node->OSTCBParent = delete_node->OSTCBParent;			//���½ڵ�ĸ���ָ��ָ���ɾ���Ľڵ�ĸ���ָ��ָ���λ��
}


//�޸�ɾ����ĺ����
static void delete_fix_up(rb_tree * tree, OS_TCB * node)
{
	OS_TCB * brothers = tree->nil;
	while (node != tree->root && node->color == BLACK)
	{
		if (node == node->OSTCBParent->OSTCBPrev)						//��ǰ�ڵ����丸�׵������
		{
			brothers = node->OSTCBParent->OSTCBNext;
								
			if (RED == brothers->color)						//case 1�� node���ֵ��Ǻ�ɫ�ģ�ͨ��
			{
				brothers->color = BLACK;					//case 1�����ֵܽڵ��ú�
				brothers->OSTCBParent->color = RED;				//case 1�����׽ڵ��ú�
				left_rotate(tree, node->OSTCBParent);			//case 1���Ը��׽ڵ��������
				brothers = node->OSTCBParent->OSTCBNext;				//case 1�������ֵܽڵ�λ��
			}												//case 1�����������1֮��w.color== BLACK �� ����ͱ��2 3 4 ��
																		
			if (brothers->OSTCBPrev->color == BLACK && brothers->OSTCBNext->color == BLACK)		//case 2�� node���ֵ��Ǻ�ɫ�ģ���������Ӷ��Ǻ�ɫ�ġ�
			{
				if (node->OSTCBParent->color == RED)				//case 2��������׽ڵ�Ϊ��ɫ
				{
					node->OSTCBParent->color = BLACK;			//case 2�������׽ڵ��ú�
					brothers->color = RED;					//case 2���ֵܽڵ��ú�
					break;									//case 2���˳�ѭ��
				}
				else
				{
					brothers->color = RED;					//case 2���ֵܽڵ��ú죬�Ը��׽ڵ�Ϊ�µĻ�׼����н���									
					node = node->OSTCBParent;					//case 2��x.p������ƽ��ģ�����x.p������һ���ڽ�㣬���԰�x.p��Ϊ�µ�x����ѭ��
					continue;								//case 2��������һ��ѭ��
				}
			}

			
			if (brothers->OSTCBNext->color == BLACK)			//case 3���ֵܽڵ�Ϊ��ɫ�ģ�����Ϊ��ɫ�����ߵ���һ����˵��w���Ҳ�ͬʱΪ��ɫ����
			{
				brothers->OSTCBPrev->color = BLACK;				//case 3�����ֵܽڵ��������ú�
				brothers->color = RED;						//case 3���ֵܽڵ��ú�
				right_rotate(tree, brothers);				//case 3������
				brothers = node->OSTCBParent->OSTCBNext;				//case 3���ֵܽڵ�����
			}

			

			brothers->color = node->OSTCBParent->color;			//case 4�� �ߵ���һ��˵���ֵܽڵ�Ϊ��ɫ�� �ֵܽڵ������Ϊ��ɫ�� �Һ���Ϊ��ɫ��
			node->OSTCBParent->color = BLACK;					//case 4�� �����׽ڵ��ú�
			brothers->OSTCBParent->color = BLACK;					//case 4�� �ֵܵ��Ҷ��ӽڵ��ú�
			left_rotate(tree, node->OSTCBParent);				//case 4�� �Ը��׽ڵ��������
			node = tree->root;								//case 4�� ��node��Ϊ���ڵ�
		}

		else{												//��ǰ�ڵ����丸�׵��Ҷ���
			brothers = node->OSTCBParent->OSTCBPrev;					//�ҵ��ֵܽڵ��λ��
			//1
			if (brothers->color == RED)						//case 1������ֵܽڵ�Ϊ��ɫ
			{
				brothers->color = BLACK;					//case 1�������ֵܽڵ���Ϊ��ɫ
				node->OSTCBParent->color = RED;					//case 1�����丸�׽ڵ���Ϊ��ɫ
				right_rotate(tree, node->OSTCBParent);			//case 1�����丸�׽ڵ��������
				brothers = node->OSTCBParent->OSTCBPrev;				//case 1�������ֵܽڵ�λ��
			}
			//2
			if (brothers->OSTCBPrev->color == BLACK && brothers->OSTCBPrev->color == BLACK)	//case 2�� node���ֵ��Ǻ�ɫ�ģ���������Ӷ��Ǻ�ɫ�ġ�
			{
				if (node->OSTCBParent->color == RED)				//case 2������丸�׽ڵ�Ϊ��ɫ
				{													
					node->OSTCBParent->color = BLACK;			//case 2�����丸�׽ڵ���Ϊ��ɫ
					brothers->color = RED;					//case 2�����ֵܽڵ���Ϊ��ɫ
					break;									//case 2������ѭ��
				}
				else										//case 2���丸�׽ڵ�Ϊ��ɫ
				{
					node->OSTCBParent->color = BLACK;			//case 2�����丸�׽ڵ�Ⱦ�ɺ�ɫ
					brothers->color = RED;					//case 2�������ֵܽڵ���Ϊ��ɫ
					node = node->OSTCBParent;					//case 2������ǰ����������λ��
					continue;								//������һ��ѭ��
				}
			}

			//3
			if (brothers->OSTCBPrev->color == BLACK)				//case 3������ֵ������Ϊ��ɫ
			{
				brothers->color = RED;						//case 3�����ֵܽڵ��ú�
				brothers->OSTCBNext->color = BLACK;				//case 3���Ҷ����ú�
				brothers = node->OSTCBParent->OSTCBPrev;				//case 3�������ֵܽڵ�
			}

			//4
			brothers->color = brothers->OSTCBParent->color;		//case 4�����ֵ���ɫ��Ϊ�丸�׵���ɫ
			node->OSTCBParent->color = BLACK;					//case 4�����丸�׽ڵ��ú�
			brothers->OSTCBPrev->color = BLACK;					//case 4���ֵܵ�������ú�
			right_rotate(tree, node->OSTCBParent);				//case 4�����丸�׽ڵ��������
			node = tree->root;								//case 4����node��Ϊ���ڵ�
		}
	}
	node->color = BLACK;									//����ǰ�����Ϊ��ɫ
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

//ɾ�������
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

	delete_node = get_node(tree, prio);								// ��ɾ���ڵ�


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

//��ʼ�������
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

