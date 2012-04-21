#include "prefix_tree.h"
#include <string.h>

#define KEY_SIZE 100
#define LINK_LIMIT 10
#define MAX_CHAR_COUNT 256

typedef enum {
    ITERATOR_DEREFERENCABLE,
    ITERATOR_BEFORE_FIRST,
    ITERATOR_PAST_REAR,   
}   IteratorTypeT;

typedef struct Node {
    struct Node *parentNode;
    struct Node *link[LINK_LIMIT];
    char key;
    LSQ_BaseTypeT value;
}   NodeT, *NodePtrT;

typedef struct {
    int size;
    NodePtrT root;   
}   TreeT, *TreePtrT;

typedef struct {
    IteratorTypeT type;
    NodePtrT node;
    TreePtrT tree;
}   IteratorT, *IteratorPtrT;

static IteratorPtrT CreateIterator(const LSQ_HandleT handle, const NodePtrT node, const IteratorTypeT type);

static int IsHaveChild(const NodePtrT node);

static void DeleteSubtree(const NodePtrT node);
static void DeleteNode(NodePtrT node);

static NodePtrT GetNodeByKey(const NodePtrT node, const LSQ_KeyT key);
static NodePtrT CreateNode(const char key, const LSQ_BaseTypeT value, const NodePtrT parent);
static NodePtrT GetChildNodeWithIdenticalKey(const NodePtrT node, const char key);
static NodePtrT GoToMinimalNode(const NodePtrT node);
static NodePtrT GoToMaximalNode(const NodePtrT node);
static NodePtrT GetRightNeighbour(const NodePtrT node);
static NodePtrT GetLeftNeighbour(const NodePtrT node);

/* Функция, создающая и возвращающая итератор */
static IteratorPtrT CreateIterator(const LSQ_HandleT handle, const  NodePtrT node, const IteratorTypeT type){
    IteratorPtrT iterator = (IteratorPtrT)malloc(sizeof(IteratorT));

    if(iterator == NULL) return NULL;
    iterator->tree = (TreePtrT)handle;
    iterator->node = node;
    iterator->type = type;
    return iterator;
}
/* Функция, ищущая узел по ключу и возвращающая его указатель */
static NodePtrT GetNodeByKey(const NodePtrT node, const LSQ_KeyT key) {
    NodePtrT iterator = node, n = NULL;
    int i, size = strlen(key);
    
    for(i = 0; i < size; i++) {
        n = GetChildNodebYKey(iterator, key[i]);
        if(n == NULL) return NULL;   
        else iterator = n;  
    }
    return iterator;
}
/* Функция, создающая узел. Возвращает указатель на него */
static NodePtrT CreateNode(const char key, const LSQ_BaseTypeT value, const NodePtrT parentNode){
    NodePtrT node = (NodePtrT)malloc(sizeof(NodeT));
	int i;

    if(node == NULL) return NULL;
    node->value = value;
    node->key = key;
    node->parentNode = parentNode;
    
    for(i = 0; i < LINK_LIMIT; i++) 
        node->link[i] = NULL;
    return node;
}
/* Функция, удаляющая данный узел вместе с его потомками */
static void DeleteSubtree(const NodePtrT node){
	int i;
    if(node == NULL) return;
    for(i = 0; i < LINK_LIMIT; i++) 
        if(node->link[i] != NULL) 
            DeleteSubtree(node->link[i]);    
    free(node);
}
/* Функция, проверяющая, есть ли у данного узла потомки */
static int IsHaveChild(const NodePtrT node) {
	int result = 0;
    for(int i = 0; i < LINK_LIMIT && !result; i++) result |= (node->link[i] != NULL);
    return result;
}
/* Функция, пробегающая до листа с приоритетом на минимальный ключ. Возвращает узел с лексикографически малым ключом */
static NodePtrT GoToMinimalNode(const NodePtrT node) {
    int i, index = -1;
    char minimalKey = 'z';

    for(i = 0; i < LINK_LIMIT; i++) {
        if(node->link[i] != NULL && node->link[i]->key < minimalKey) {
            index = i;
            minimalKey = node->link[i]->key;
        }
    }
    if(index != -1) return GoToMinimalNode(node->link[index]);
    else return node;
}
/* Функция, пробегающая до листа с приоритетом на максимальный ключ. Возвращает узел с лексикографически большим ключом */
static NodePtrT GoToMaximalNode(const NodePtrT node) {
    int i, index = -1;
    char maximalKey = '/';
    
    for(i = 0; i < LINK_LIMIT; i++) {
        if(node->link[i] != NULL && node->link[i]->key > maximalKey) {
            index = i;
            maximalKey = node->link[i]->key;
        }
    }
    if(index != -1) return GoToMaximalNode(node->link[index]);
    else return node;
}
/* Функция, возвращающая правого соседа данного узла. Если соседа нет, то возвращает NULL */
static NodePtrT GetRightNeighbour(const NodePtrT node) {
    NodePtrT rightNode = NULL, parent = node->parentNode;
    char key = node->key;
    int i, differenceKeyCount, minimalDifference = MAX_CHAR_COUNT;
    
    for(i = 0; i < LINK_LIMIT; i++) {
        if(parent->link[i] != NULL && key < parent->link[i]->key) {
            differenceKeyCount = parent->link[i]->key - key;
            if(differenceKeyCount < minimalDifference) {
                minimalDifference = differenceKeyCount;
                rightNode = parent->link[i];
            }
        }
    }
    if(minimalDifference != MAX_CHAR_COUNT) return GoToMinimalNode(rightNode);
    else return NULL;
}
/* Функция, возвращающая правого соседа данного узла. Если соседа нет, то возвращает NULL */
static NodePtrT GetLeftNeighbour(const NodePtrT node) {
    NodePtrT leftNode = NULL, parent = node->parentNode;
    char key = node->key;
    int i, differenceKeyCount, minimalDifference = MAX_CHAR_COUNT;
    
    /*for(i = 0; i < LINK_LIMIT; i++) {
        if(parent->link[i] != NULL && key > parent->link[i]->key) {
            differenceKeyCount = key - parent->link[i]->key;
            if(differenceKeyCount < minimalDifference) {
                minimalDifference = differenceKeyCount;
                leftNode = parent->link[i];
            }
        }
    }*/
	int leftSibling;
	for (leftSibling = key - 1; leftSibling >= '0' && parent->link[leftSibling] == NULL; --leftSibling);
    if(minimalDifference != MAX_CHAR_COUNT) return GoToMaximalNode(leftNode);
    else return NULL;
}
/* Функция, удалающая узел, а также удаляющая родительские элементы без значения */
static void DeleteNode(NodePtrT node) {
    NodePtrT parent = node->parentNode;
	int i, index;

    if(parent == NULL) return; 

    for(i = 0; i < LINK_LIMIT; i++) 
        if(parent->link[i] != NULL && parent->link[i]->key == node->key) 
            index = i; 

    parent->link[index] = NULL;
	free(node);
    node = NULL;
    if(!IsHaveChild(parent) && parent->value == NULL) DeleteNode(parent);
}
/* Функция, возвращающая потомка данного узла с данным ключом. Если такового нет, то возвращает NULL */
static NodePtrT GetChildNodeWithIdenticalKey(const NodePtrT node, const char key) {
    int i;
	if(node == NULL) return NULL;
    for(i = 0; i < LINK_LIMIT; i++) {
        if(node->link[i] != NULL && node->link[i]->key == key) 
            return node->link[i];
    }
    return NULL;
}

extern LSQ_HandleT LSQ_CreateSequence(void) {
    TreePtrT tree = (TreePtrT)malloc(sizeof(TreeT));
    if(tree == NULL) return LSQ_HandleInvalid;
    tree->root = NULL;
    tree->size = 0;
    return tree;
}

extern void LSQ_DestroySequence(LSQ_HandleT handle) {
	if(handle == LSQ_HandleInvalid) return;
    DeleteSubtree(((TreePtrT)handle)->root);
    free(handle);
}

extern LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle) {
    if(handle == LSQ_HandleInvalid) return 0;
    return ((TreePtrT)handle)->size;
}

extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator) {
    if(iterator == NULL) return 0;
    return ((IteratorPtrT)iterator)->type == ITERATOR_DEREFERENCABLE;
}

extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator) {
    if(iterator == NULL) return 0;
    return ((IteratorPtrT)iterator)->type == ITERATOR_PAST_REAR;
} 

extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator) {
    if(iterator == NULL) return 0;
    return ((IteratorPtrT)iterator)->type == ITERATOR_BEFORE_FIRST;
}

extern LSQ_BaseTypeT LSQ_DereferenceIterator(LSQ_IteratorT iterator) {
    if(iterator == NULL || ((IteratorPtrT)iterator)->node == NULL) return 0;
    return ((IteratorPtrT)iterator)->node->value;
}

extern char* LSQ_GetIteratorKey(LSQ_IteratorT iterator) {
	NodePtrT node = NULL;
	char *key, *revKey;
	int i = 0, len;

	key = calloc(KEY_SIZE, sizeof(char));
	revKey = calloc(KEY_SIZE, sizeof(char));

	for(i = 0; i < KEY_SIZE; i++) {
		key[i] = NULL;
		revKey[i] = NULL;
	}

	node = ((IteratorPtrT)iterator)->node;
	if(node == NULL) return NULL;

    if(iterator == NULL || ((IteratorPtrT)iterator)->node == NULL) return 0;

	for(i = 0; i < KEY_SIZE; i++) 
		if(node->parentNode != NULL) {
			key[i] = node->key;
			node = node->parentNode;
		}

	len = strlen(key);
	for(i = 0; i < len; i++)
		revKey[i] = key[len - i - 1];
	free(key);
    return revKey;
}

extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_KeyT key) {
	NodePtrT node = GetNodeByKey(((TreePtrT)handle)->root, key);

    if(handle == LSQ_HandleInvalid) return NULL;
    if(node == NULL) return LSQ_GetPastRearElement(handle);
    return CreateIterator(handle, node, ITERATOR_DEREFERENCABLE);
}

extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle) {
	IteratorPtrT iterator = NULL;

    if(handle == LSQ_HandleInvalid) return NULL;
	iterator = CreateIterator(handle, NULL, ITERATOR_BEFORE_FIRST);
    if(iterator == NULL) return NULL;

    LSQ_AdvanceOneElement(iterator);
    return iterator;
}

extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle) {
    if(handle == LSQ_HandleInvalid) return NULL;
    return CreateIterator(handle, NULL, ITERATOR_PAST_REAR);
}

extern void LSQ_DestroyIterator(LSQ_IteratorT iterator) {
    free(iterator);
}

extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator) {
    IteratorPtrT iter = (IteratorPtrT)iterator;
	NodePtrT node = iter->node;
    NodePtrT rightNeighbour = NULL;

    if(iter == NULL || iter->type == ITERATOR_PAST_REAR) return;
    
    if(iter->type == ITERATOR_BEFORE_FIRST) {
        if(iter->tree->root == NULL)
            iter->type = ITERATOR_PAST_REAR;
        else {
            iter->node = GoToMinimalNode(iter->tree->root);
            iter->type = ITERATOR_DEREFERENCABLE;
        }
        return;
    }
    
    while(node->parentNode != NULL && (rightNeighbour = GetRightNeighbour(node)) == NULL) {
        node = node->parentNode;    
    }
    
    if(rightNeighbour != NULL) {
        iter->node = rightNeighbour;    
    }
    else {
        iter->type = ITERATOR_PAST_REAR;
        iter->node = NULL;
    }
}

extern void LSQ_RewindOneElement(LSQ_IteratorT iterator) {
    IteratorPtrT iter = (IteratorPtrT)iterator;
	NodePtrT node = iter->node;
    NodePtrT leftNeighbour = NULL;

    if(iter == NULL || iter->type == ITERATOR_BEFORE_FIRST) return;
    
    if(iter->type == ITERATOR_PAST_REAR) {
        if(iter->tree->root == NULL)
            iter->type = ITERATOR_BEFORE_FIRST;
        else {
            iter->node = GoToMaximalNode(iter->tree->root);
            iter->type = ITERATOR_DEREFERENCABLE;
        }
        return;
    }
    
    while(node->parentNode != NULL && (leftNeighbour = GetLeftNeighbour(node)) == NULL) {
        node = node->parentNode;    
    }
    
    if(leftNeighbour != NULL) {
        iter->node = leftNeighbour;    
    }
    else {
        iter->type = ITERATOR_BEFORE_FIRST;
        iter->node = NULL;
    }
}

extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift) {
    if(iterator == NULL) return;
    for(; shift > 0; LSQ_AdvanceOneElement(iterator)) shift--;
    for(; shift < 0; LSQ_RewindOneElement(iterator)) shift++;
}

extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos) {
    if(iterator == NULL) return;
    ((IteratorPtrT)iterator)->type = ITERATOR_BEFORE_FIRST;
    LSQ_ShiftPosition(iterator, pos + 1);
}

extern void LSQ_InsertElement(LSQ_HandleT handle, LSQ_KeyT key, LSQ_BaseTypeT value) {
    TreePtrT trie = (TreePtrT)handle;
    NodePtrT n = NULL, node = trie->root;
	int i, j, size = strlen(key);
    
	if(handle == LSQ_HandleInvalid) return;

    if(node == NULL) { 
        node = CreateNode("", NULL, NULL);  
        trie->root = node;
    }
    
    for(i = 0; i < size; i++) {
        n = GetChildNodeWithIdenticalKey(node, key[i]);
        if(n == NULL) {
            for(j = 0; j < LINK_LIMIT; j++) 
                if(node->link[j] == NULL) {
                    node->link[j] = CreateNode(key[i], NULL, node);   
                    node = node->link[j]; 
                    break;    
                }
        }
        else node = n; 
    }
    node->value = value;
    trie->size++;
}

extern void LSQ_DeleteFrontElement(LSQ_HandleT handle) {
    IteratorPtrT iterator = NULL; 
	char *key = NULL;

    if(handle == LSQ_HandleInvalid) return;
	iterator = LSQ_GetFrontElement(handle);
	if(iterator == NULL) return;
	
	key = LSQ_GetIteratorKey(iterator);
    LSQ_DeleteElement(handle, key);  
    LSQ_DestroyIterator(iterator);
	free(key);
}

extern void LSQ_DeleteRearElement(LSQ_HandleT handle) {
    IteratorPtrT iterator = NULL; 
	char *key = NULL;
	
    if(handle == LSQ_HandleInvalid) return;
	iterator = LSQ_GetPastRearElement(handle);
	if(iterator == NULL) return;

    LSQ_RewindOneElement(iterator);
	key = LSQ_GetIteratorKey(iterator);
    LSQ_DeleteElement(handle, key);
    LSQ_DestroyIterator(iterator); 
	free(key);
}

extern void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_KeyT key) {
    TreePtrT trie = NULL; 
    NodePtrT node = NULL; 

	if(handle == LSQ_HandleInvalid) return;
	trie = (TreePtrT)handle;
	node = GetNodeByKey(trie->root, key);
    if(node == NULL) return;

	if(!IsHaveChild(node)) 
		DeleteNode(node); 
    else
        node->value = NULL;
    trie->size--;
}
