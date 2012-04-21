#include "linear_sequence_assoc_hash.h"
#include <stdlib.h>
#include <math.h>

#define SIZE_OF_TABLE 1000

typedef enum IteratorType {
    ITERATOR_DEREFERENCABLE,
    ITERATOR_PAST_REAR,
    ITERATOR_BEFORE_FIRST,
}   IteratorTypeT;

typedef struct Pair {
    LSQ_KeyT key;
    LSQ_BaseTypeT value;
    struct Pair *next;
}   PairT, *PairPtrT;

typedef struct Table {
    LSQ_SizeT size;
    PairPtrT *element;
    
    LSQ_Callback_CloneFuncT *kcloneFunction;
    LSQ_Callback_SizeFuncT *ksizeFunction;
    LSQ_Callback_CompareFuncT *kcompareFunction;
    LSQ_Callback_CloneFuncT *vcloneFunction;
}   TableT, *TablePtrT;

typedef struct Iterator {
    IteratorTypeT type;
    TablePtrT table;
    PairPtrT element;
}   IteratorT, *IteratorPtrT;

static int CalculateHash(TablePtrT table, LSQ_KeyT key);
static LSQ_IteratorT CreateIterator(LSQ_HandleT handle, IteratorTypeT type, PairPtrT element);

int CalculateHash(TablePtrT table, LSQ_KeyT key) {
    int i, size = table->ksizeFunction(key);
    double intPart, M = SIZE_OF_TABLE - 1, K = 0.618003;
    unsigned int X = 0;
    
    for(i = 0; i < size; ++i) 
        X += ((char*)key)[i] * i;

    return (int)(M * modf(K * X, &intPart)); 
}

extern LSQ_HandleT LSQ_CreateSequence(LSQ_Callback_CloneFuncT keyCloneFunc, LSQ_Callback_SizeFuncT keySizeFunc, 
                                      LSQ_Callback_CompareFuncT keyCompFunc, LSQ_Callback_CloneFuncT valCloneFunc) 
{    
    TablePtrT table = (TablePtrT)malloc(sizeof(TableT));
    if(table == NULL) return LSQ_HandleInvalid;
    table->size = 0;
    table->element = (PairPtrT*)calloc(SIZE_OF_TABLE, sizeof(PairT));
    
    if(table->element == NULL) {
        free(table);   
        return LSQ_HandleInvalid;
    }
    table->kcloneFunction   = keyCloneFunc;
    table->kcompareFunction = keyCompFunc;
    table->ksizeFunction    = keySizeFunc;
    table->vcloneFunction   = valCloneFunc;
    return table;
}

extern void LSQ_DestroySequence(LSQ_HandleT handle) {
    if(handle == LSQ_HandleInvalid) return;
    PairPtrT element = NULL, previousElement = NULL;
    TablePtrT table = (TablePtrT)handle;
    int i;
    
    for(i = 0; i < SIZE_OF_TABLE; i++) {
        element = table->element[i];
        while(element != NULL) {
            previousElement = element;
            element = element->next;
            free(previousElement->value);
            free(previousElement->key);
            free(previousElement);
        }
    }
    free(table->element);
    free(table);
}

extern LSQ_SizeT LSQ_GetSize(LSQ_HandleT handle) {
    if(handle == LSQ_HandleInvalid) return 0;
    return ((TablePtrT)handle)->size;
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
    if(iterator == NULL || !LSQ_IsIteratorDereferencable(iterator)) return NULL;
    return ((IteratorPtrT)iterator)->element->value;    
}

extern LSQ_KeyT LSQ_GetIteratorKey(LSQ_IteratorT iterator) {
    if(iterator == NULL || !LSQ_IsIteratorDereferencable(iterator)) return NULL;
    return ((IteratorPtrT)iterator)->element->key;    
}

LSQ_IteratorT CreateIterator(LSQ_HandleT handle, IteratorTypeT type, PairPtrT element) {
    if(handle == LSQ_HandleInvalid) return NULL;
    IteratorPtrT iterator = (IteratorPtrT)malloc(sizeof(IteratorT));
    if(iterator == NULL) return NULL;
    
    iterator->table = (TablePtrT)handle;
    iterator->type = type;
    iterator->element = element;
    return iterator;
}

extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_KeyT key) {
    if(handle == LSQ_HandleInvalid) return NULL;   
    int index = CalculateHash(handle, key);
    TablePtrT table = (TablePtrT)handle;
    PairPtrT element = table->element[index];
        
    while(element != NULL && table->kcompareFunction(element->key, key)) 
        element = element->next; 
        
    if(element == NULL) return LSQ_GetPastRearElement(handle);
    return CreateIterator(handle, ITERATOR_DEREFERENCABLE, element);
}

extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle) {
    if(handle == LSQ_HandleInvalid) return NULL;  
    TablePtrT table = (TablePtrT)handle; 
    int i = 0;
    
    for(; i < SIZE_OF_TABLE && table->element[i] == NULL; i++);
        
    if(i == SIZE_OF_TABLE) return LSQ_GetPastRearElement(handle);
    return CreateIterator(handle, ITERATOR_DEREFERENCABLE, table->element[i]);
}

extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle) {
    return CreateIterator(handle, ITERATOR_PAST_REAR, NULL);
}

extern void LSQ_DestroyIterator(LSQ_IteratorT iterator) {
    free(iterator);
}

extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator) {
    if(iterator == NULL || LSQ_IsIteratorPastRear(iterator)) return;
    IteratorPtrT iter = (IteratorPtrT)iterator;
    
    if(iter->element->next != NULL) {
        iter->element = iter->element->next;
        return;
    }
    
    int index = CalculateHash(iter->table, iter->element->key) + 1;
    while(index < SIZE_OF_TABLE && iter->table->element[index] == NULL) index++;
        
    if(index == SIZE_OF_TABLE) {
        iter->type = ITERATOR_PAST_REAR;
        iter->element = NULL;   
    }
    else {
        iter->element = iter->table->element[index];
    }
}

extern void LSQ_InsertElement(LSQ_HandleT handle, LSQ_KeyT key, LSQ_BaseTypeT value) {
    if(handle == LSQ_HandleInvalid) return;
    TablePtrT table = (TablePtrT)handle;
    PairPtrT element = NULL;
    IteratorPtrT iterator = LSQ_GetElementByIndex(handle, key);
    
    if(iterator != NULL && LSQ_IsIteratorDereferencable(iterator)) {
        free(iterator->element->value);   
        iterator->element->value = table->vcloneFunction(value);
        return;
    }
    
    int index = CalculateHash(handle, key);
    
    element = (PairPtrT)malloc(sizeof(PairT));
    if(element == NULL) return;

    element->key = table->kcloneFunction(key);
    element->value = table->vcloneFunction(value);
    element->next = table->element[index];
    
    table->element[index] = element;
    table->size++;
}

extern void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_KeyT key) {
    if(handle == LSQ_HandleInvalid) return;
    TablePtrT table = (TablePtrT)handle;
    PairPtrT element = NULL, previousElement = NULL;
    
    int index = CalculateHash(table, key);
    element = table->element[index];
    
    while(element != NULL && table->kcompareFunction(element->key, key)) {
        previousElement = element;
        element = element->next;
    }
    
    if(element == NULL) return;
    
    if(previousElement == NULL) 
        table->element[index] = element->next;
    else                        
        previousElement->next = element->next;
        
    free(element->key);
    free(element->value);
    free(element);
    table->size--;
}
