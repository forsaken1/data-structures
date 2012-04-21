#include "linear_sequence_assoc.h"

typedef enum {
    ITERATOR_DEREFERENCABLE,
    ITERATOR_BEFORE_FIRST,
    ITERATOR_PAST_REAR,   
}   IteratorTypeT;

typedef struct Node {
    struct Node *parentNode;
    struct Node *leftNode;
    struct Node *rightNode;
    LSQ_IntegerIndexT key;
    LSQ_BaseTypeT value;
    int height;
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

static IteratorPtrT CreateIterator(LSQ_HandleT handle, NodePtrT node, IteratorTypeT type);

static NodePtrT GetNodeByIndex(NodePtrT node, LSQ_IntegerIndexT key);
static NodePtrT GetLeftLeaf(NodePtrT node);
static NodePtrT GetRightLeaf(NodePtrT node);
static NodePtrT CreateNode(LSQ_IntegerIndexT key, LSQ_BaseTypeT value, NodePtrT parent);
static NodePtrT GoToLeaf(NodePtrT node, LSQ_IntegerIndexT key);

static void ReplaceNode(TreePtrT tree, NodePtrT node, NodePtrT new_node);
static void DeleteNode(NodePtrT node);
static void RefreshNodeHeight(NodePtrT node);
static void LeftRotate(NodePtrT node, TreePtrT tree);
static void RightRotate(NodePtrT node, TreePtrT tree);
static void Balance(TreePtrT tree, NodePtrT node);

static int GetNodeHeight(NodePtrT node);
static int NodeBalanceParameter(NodePtrT node);
static int Max(int a, int b);

static IteratorPtrT CreateIterator(LSQ_HandleT handle, NodePtrT node, IteratorTypeT type){
    IteratorPtrT iterator = (IteratorPtrT)malloc(sizeof(IteratorT));
    if(iterator == NULL) return NULL;
    iterator->tree = (TreePtrT)handle;
    iterator->node = node;
    iterator->type = type;
    return iterator;
}

static NodePtrT GetNodeByIndex(NodePtrT node, LSQ_IntegerIndexT key){
    while(node != NULL && node->key != key)
        if(node->key < key)
            node = node->rightNode;
        else
            node = node->leftNode;
    return node;
}

static NodePtrT GetLeftLeaf(NodePtrT node) {
    while(node->leftNode != NULL)
        node = node->leftNode;
    return node;
}

static NodePtrT GetRightLeaf(NodePtrT node) {
    while(node->rightNode != NULL)
        node = node->rightNode;
    return node;
}

static NodePtrT CreateNode(LSQ_IntegerIndexT key, LSQ_BaseTypeT value, NodePtrT parentNode){
    NodePtrT node = (NodePtrT)malloc(sizeof(NodeT));
    if(node == NULL) return NULL;
    node->key = key;
    node->value = value;
    node->parentNode = parentNode;
    node->leftNode = NULL;
    node->rightNode = NULL;
    return node;
}

static NodePtrT GoToLeaf(NodePtrT node, LSQ_IntegerIndexT key) {
    if(key < node->key) {
        if(node->leftNode != NULL)
            return GoToLeaf(node->leftNode, key);
        else
            return node;
    }
    else 
        if(key > node->key) {
            if(node->rightNode != NULL) 
                return GoToLeaf(node->rightNode, key);
            else
                return node;
        }
        else
            return node;
}

static void ReplaceNode(TreePtrT tree, NodePtrT node, NodePtrT newNode){
    if(newNode != NULL)
        newNode->parentNode = node->parentNode;
        
    if(node->parentNode == NULL)
        tree->root = newNode;
    else
        if(node->parentNode->leftNode == node)
            node->parentNode->leftNode = newNode;
        else
            node->parentNode->rightNode = newNode;
}

static void DeleteNode(NodePtrT node){
    if(node == NULL) return;
    DeleteNode(node->leftNode);
    DeleteNode(node->rightNode);
    free(node);
}

static int GetNodeHeight(NodePtrT node) {
    return node == NULL ? 0 : node->height;
}

static int NodeBalanceParameter(NodePtrT node) {
    return GetNodeHeight(node->leftNode) - GetNodeHeight(node->rightNode);
}

static int Max(int a, int b) {
    return a > b ? a : b;   
}

static void RefreshNodeHeight(NodePtrT node) {
    node->height = 1 + Max(GetNodeHeight(node->leftNode), GetNodeHeight(node->rightNode));
}

static void LeftRotate(NodePtrT node, TreePtrT tree){
    NodePtrT newRoot = node->rightNode;
    ReplaceNode(tree, node, newRoot);
    node->rightNode = newRoot->leftNode;
    if(node->rightNode != NULL)
        node->rightNode->parentNode = node;
    node->parentNode = newRoot;
    newRoot->leftNode = node;
    RefreshNodeHeight(node);
    RefreshNodeHeight(newRoot);
}

static void RightRotate(NodePtrT node, TreePtrT tree){
    NodePtrT newRoot = node->leftNode;
    ReplaceNode(tree, node, newRoot);
    node->leftNode = newRoot->rightNode;
    if(node->leftNode != NULL)
        node->leftNode->parentNode = node;
    node->parentNode = newRoot;
    newRoot->rightNode = node;
    RefreshNodeHeight(node);
    RefreshNodeHeight(newRoot);
}

static void Balance(TreePtrT tree, NodePtrT node) {
    NodePtrT parent = NULL;
    int nodeBalance;
    while(node != NULL) {
        RefreshNodeHeight(node);
        nodeBalance = NodeBalanceParameter(node);
        parent = node->parentNode;
        if(abs(nodeBalance) <= 1) 
            return;
        else
            if(nodeBalance < -1) {
                if(NodeBalanceParameter(node->rightNode) > 0)
                    RightRotate(node->rightNode, tree);
                LeftRotate(node, tree);
            }
            else
                if(nodeBalance > 1) {
                    if(NodeBalanceParameter(node->leftNode) < 0)
                        LeftRotate(node->leftNode, tree);
                    RightRotate(node, tree);
                }
        node = parent;
    }
}

extern LSQ_HandleT LSQ_CreateSequence(void) {
    TreePtrT tree = (TreePtrT)malloc(sizeof(TreeT));
    if(tree == NULL) return LSQ_HandleInvalid;
    tree->root = NULL;
    tree->size = 0;
    return tree;
}

extern void LSQ_DestroySequence(LSQ_HandleT handle) {
    DeleteNode(((TreePtrT)handle)->root);
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

extern LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator) {
    if(iterator == NULL || ((IteratorPtrT)iterator)->node == NULL) return NULL;
    return &(((IteratorPtrT)iterator)->node->value);
}

extern LSQ_IntegerIndexT LSQ_GetIteratorKey(LSQ_IteratorT iterator) {
    if(iterator == NULL || ((IteratorPtrT)iterator)->node == NULL) return -1;
    return ((IteratorPtrT)iterator)->node->key;
}

extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index) {
    if(handle == LSQ_HandleInvalid) return NULL;
    NodePtrT node = GetNodeByIndex(((TreePtrT)handle)->root, index);
    if(node == NULL)
        return LSQ_GetPastRearElement(handle);
    return CreateIterator(handle, node, ITERATOR_DEREFERENCABLE);
}

extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle) {
    if(handle == LSQ_HandleInvalid) return NULL;
    IteratorPtrT iterator = CreateIterator(handle, NULL, ITERATOR_BEFORE_FIRST);
    if(iterator == NULL) return NULL;
    LSQ_AdvanceOneElement(iterator);
    return iterator;
}

extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle) {
    return CreateIterator(handle, NULL, ITERATOR_PAST_REAR);
}

extern void LSQ_DestroyIterator(LSQ_IteratorT iterator) {
    free(iterator);
}

extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator) {
    IteratorPtrT iter = (IteratorPtrT)iterator;
    if(iter == NULL || iter->type == ITERATOR_PAST_REAR) return;
    
    if(iter->type == ITERATOR_BEFORE_FIRST) {
        if(iter->tree->root == NULL)
            iter->type = ITERATOR_PAST_REAR;
        else {
            iter->node = GetLeftLeaf(iter->tree->root);
            iter->type = ITERATOR_DEREFERENCABLE;
        }
        return;
    }
    
    NodePtrT node = iter->node;
    NodePtrT anySideNode = NULL;
    
    while(node->parentNode != NULL && node->rightNode == anySideNode) {
        anySideNode = node;
        node = node->parentNode;
    }
    
    if(node->rightNode == anySideNode) {
        iter->type = ITERATOR_PAST_REAR;
        iter->node = NULL;
    }
    else 
        if(node != iter->node)
            iter->node = node;
        else
            if(node->rightNode != NULL)
                iter->node = GetLeftLeaf(node->rightNode);
}

extern void LSQ_RewindOneElement(LSQ_IteratorT iterator) {
    IteratorPtrT iter = (IteratorPtrT)iterator;
    if(iter == NULL || iter->type == ITERATOR_BEFORE_FIRST) return;
    
    if(iter->type == ITERATOR_PAST_REAR) {
        if(iter->tree->root == NULL)
            iter->type = ITERATOR_BEFORE_FIRST;
        else {
            iter->node = GetLeftLeaf(iter->tree->root);
            iter->type = ITERATOR_DEREFERENCABLE;
        }
        return;
    }
    
    NodePtrT node = iter->node;
    NodePtrT parentNode = NULL;
    
    while(node->parentNode != NULL && node->leftNode == parentNode) {
        parentNode = node;
        node = node->parentNode;
    }
    
    if(node->leftNode == parentNode) {
        iter->type = ITERATOR_BEFORE_FIRST;
        iter->node = NULL;
    }
    else 
        if(node != iter->node)
            iter->node = node;
        else
            if(node->leftNode != NULL)
                iter->node = GetRightLeaf(node->leftNode);
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

extern void LSQ_InsertElement(LSQ_HandleT handle, LSQ_IntegerIndexT key, LSQ_BaseTypeT value) {
    if(handle == LSQ_HandleInvalid) return;
    TreePtrT tree = (TreePtrT)handle;
    
    if(tree->root == NULL) {
        tree->root = CreateNode(key, value, NULL);
        if(tree->root == NULL) return;
        tree->size++;
    }
    
    NodePtrT parent = GoToLeaf(tree->root, key);
    if(parent->key == key) { 
        parent->value = value;
        return;
    }
        
    NodePtrT node = CreateNode(key, value, parent);
    if(node == NULL) return;
    
    tree->size++;
    if(key < parent->key)
        parent->leftNode = node;
    else
        parent->rightNode = node;
        
    Balance(tree, parent);
}

extern void LSQ_DeleteFrontElement(LSQ_HandleT handle) {
    IteratorPtrT iterator = LSQ_GetFrontElement(handle);
    if(handle == LSQ_HandleInvalid || iterator == NULL) return;
    LSQ_DeleteElement(handle, LSQ_GetIteratorKey(iterator));    
    LSQ_DestroyIterator(iterator);
}

extern void LSQ_DeleteRearElement(LSQ_HandleT handle) {
    IteratorPtrT iterator = LSQ_GetPastRearElement(handle);
    if(handle == LSQ_HandleInvalid || iterator == NULL) return;
    LSQ_RewindOneElement(iterator);
    LSQ_DeleteElement(handle, LSQ_GetIteratorKey(iterator));    
    LSQ_DestroyIterator(iterator);    
}

extern void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_IntegerIndexT key) {
    TreePtrT tree = (TreePtrT)handle;
    if(handle == LSQ_HandleInvalid || tree->root == NULL) return;
    
    NodePtrT node = GetNodeByIndex(tree->root, key);
    if(node == NULL) return;
    
    NodePtrT parentNode = node->parentNode;
    NodePtrT leftNode = NULL;
    int keyValue;
    
    if(node->leftNode == NULL && node->rightNode == NULL)
        ReplaceNode(tree, node, NULL);
    else
        if(node->leftNode != NULL && node->rightNode != NULL) {
            leftNode = GetLeftLeaf(node->rightNode);
            keyValue = leftNode->key;
            node->value = leftNode->value;
            LSQ_DeleteElement(handle, leftNode->key);
            node->key = keyValue;
            return;
        }
        else
            if(node->leftNode != NULL)
                ReplaceNode(tree, node, node->leftNode);
            else
                if(node->rightNode != NULL)
                    ReplaceNode(tree, node, node->rightNode);
                    
    free(node);
    tree->size--;
    Balance(tree, parentNode);
}
