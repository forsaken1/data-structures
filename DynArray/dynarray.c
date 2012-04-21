#include "linear_sequence.h"
#include <mem.h>
#define INITIAL_SEQUENCE_SIZE 1
#define ENLARGE_SEQUENSE_MULTIPLIER 2
#define REDUSE_SEQUENSE_MULTIPLIER 2
#define FACTOR_FOR_REDUCTION_SEQUENSE 0.25

typedef struct {
    LSQ_BaseTypeT* data;
    int physicalSize, logicalSize;
}   SequenceT, *SequencePtrT;

typedef struct {
    SequencePtrT handle; 
    LSQ_IntegerIndexT index;   
}   IteratorT, *IteratorPtrT;

static void InsertElementInSequence(LSQ_HandleT, LSQ_BaseTypeT, LSQ_IntegerIndexT);
static void DeleteElementFromSequence(LSQ_HandleT, LSQ_IntegerIndexT);
static LSQ_IteratorT CreateIterator(LSQ_HandleT, LSQ_IntegerIndexT);

static void InsertElementInSequence(LSQ_HandleT handle, LSQ_BaseTypeT element, LSQ_IntegerIndexT index) {
    LSQ_BaseTypeT* IndexOfElement;
    SequencePtrT pointer = (SequencePtrT)handle;
       
    if (pointer != NULL) {
        if(pointer->logicalSize == pointer->physicalSize) {
            pointer->physicalSize *= ENLARGE_SEQUENSE_MULTIPLIER;  
            pointer->data = (LSQ_BaseTypeT*)realloc(pointer->data, sizeof(LSQ_BaseTypeT) * pointer->physicalSize);
        }
        IndexOfElement = pointer->data + index;
        memmove(IndexOfElement + 1, IndexOfElement, sizeof(LSQ_BaseTypeT) * (pointer->logicalSize - index)); 
        *IndexOfElement = element;
        pointer->logicalSize++;
    }
}

static void DeleteElementFromSequence(LSQ_HandleT handle, LSQ_IntegerIndexT index) {
    LSQ_BaseTypeT* IndexOfElement;
    SequencePtrT pointer = (SequencePtrT)handle;
      
    if(pointer != NULL) {
        if(pointer->logicalSize > 0) {
            IndexOfElement = pointer->data + index;
            memmove(IndexOfElement , IndexOfElement + 1, sizeof(LSQ_BaseTypeT) * (pointer->logicalSize - index)); 
            pointer->logicalSize--;
        }               
		if(pointer->logicalSize <= pointer->physicalSize * FACTOR_FOR_REDUCTION_SEQUENSE) {
			pointer->physicalSize /= REDUSE_SEQUENSE_MULTIPLIER;
			pointer->data = (LSQ_BaseTypeT*)realloc(pointer->data, sizeof(LSQ_BaseTypeT) * pointer->logicalSize);
		}
    }      
}

static LSQ_IteratorT CreateIterator(LSQ_HandleT handle, LSQ_IntegerIndexT index) {
    IteratorPtrT iterator = (IteratorPtrT)malloc(sizeof(IteratorT));
    
    if(iterator == NULL || handle == LSQ_HandleInvalid) 
        return LSQ_HandleInvalid;
    iterator->index = index;  
    iterator->handle = (SequencePtrT)handle; 
    return iterator; 
}

/* �������, ��������� ������ ���������. ���������� ����������� ��� ���������� */
extern LSQ_HandleT LSQ_CreateSequence(void) {
    SequencePtrT pointer = (SequencePtrT)malloc(sizeof(SequenceT));
    
    if(pointer == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;            
    pointer->data = (LSQ_BaseTypeT*)malloc(sizeof(LSQ_BaseTypeT) * INITIAL_SEQUENCE_SIZE);
    pointer->physicalSize = INITIAL_SEQUENCE_SIZE;
    pointer->logicalSize = 0;
    return pointer;
}

/* �������, ������������ ��������� � �������� ������������. ����������� ������������� ��� ������ */
extern void LSQ_DestroySequence(LSQ_HandleT handle) {    
    if (handle != LSQ_HandleInvalid) {
        free(((SequencePtrT)handle)->data);
        free(handle);
    }
}

/* �������, ������������ ������� ���������� ��������� � ���������� */
extern LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle) {
    if (handle == LSQ_HandleInvalid)
        return -1; 
    return ((SequencePtrT)handle)->logicalSize;
}

/* ��������� ��� ������� ������� �������� � ������ � ���������� ��� ���������� */
/* �������, ������������ ��������, ����������� �� ������� � ��������� �������� */
extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index) {	
    return CreateIterator(handle, index);                   
}

/* �������, ������������ ��������, ����������� �� ������ ������� ���������� */
extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle) {
	if(handle != NULL)
		return CreateIterator(handle, 0);         
}

/* �������, ������������ ��������, ����������� �� ��������� ������� ���������� */
extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle) {
    if (handle == LSQ_HandleInvalid)    
        return 0;
    return CreateIterator(handle, ((SequencePtrT)handle)->logicalSize );   
}

/* �������, ������������ �������� � �������� ������������ � ������������� ������������� ��� ������ */
extern void LSQ_DestroyIterator(LSQ_IteratorT iterator) {
    free(iterator); 
}

/* �������, ������������, ����� �� ������ �������� ���� ����������� */
extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator) {
    if (iterator == NULL)
        return 0;
    return !LSQ_IsIteratorPastRear(iterator) && !LSQ_IsIteratorBeforeFirst(iterator);
}

/* �������, ������������, ��������� �� ������ �������� �� �������, ��������� �� ��������� � ���������� */
extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator) {
    if (iterator == NULL) 
        return 0;
    return ((IteratorPtrT)iterator)->index >= ((SequencePtrT)((IteratorPtrT)iterator)->handle)->logicalSize;
}

/* �������, ������������, ��������� �� ������ �������� �� �������, �������������� ������� � ���������� */
extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator) {
    if (iterator == NULL) 
        return 0;
    return ((IteratorPtrT)iterator)->index < 0;      
}

/* ������� ���������������� ��������. ���������� ��������� �� �������, �� ������� ��������� ������ �������� */
extern LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator) {
    IteratorPtrT iter = (IteratorPtrT)iterator;
       
    if( iter == NULL || ((SequencePtrT)(iter->handle)) == LSQ_HandleInvalid )
        return NULL;
    return ((SequencePtrT)(iter->handle))->data + iter->index;
}

/* �������, ������������ �������� �� ���� ������� ������ */
extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator) {
	if(iterator != NULL)
		LSQ_ShiftPosition(iterator, 1);
}

/* �������, ������������ �������� �� ���� ������� ����� */
extern void LSQ_RewindOneElement(LSQ_IteratorT iterator) {
	if(iterator != NULL)
		LSQ_ShiftPosition(iterator, -1);
}

/* �������, ������������ �������� �� �������� �������� �� ������ */
extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift) {
    if(iterator != NULL) 
        ((IteratorPtrT)iterator)->index += shift;
}

/* �������, ��������������� �������� �� ������� � ��������� ������� */
extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos) {
    if(iterator != NULL) 
        ((IteratorPtrT)iterator)->index = pos;
}

/* �������, ����������� ������� � ������ ���������� */
extern void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element) {
    if(handle != NULL)
		InsertElementInSequence(handle, element, 0);
}

/* �������, ����������� ������� � ����� ���������� */
extern void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element) {
    if(handle != NULL)
		InsertElementInSequence(handle, element, ((SequencePtrT)handle)->logicalSize);
}

/* �������, ����������� ������� � ��������� �� �������, ����������� � ������ ������ ����������. �������, �� �������  *
 * ��������� ��������, � ����� ��� �����������, ���������� �� ���� ������� � �����.                                  */
extern void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement) {
    if(iterator != NULL)
		InsertElementInSequence(((IteratorPtrT)iterator)->handle, newElement, ((IteratorPtrT)iterator)->index);
}

/* �������, ��������� ������ ������� ���������� */
extern void LSQ_DeleteFrontElement(LSQ_HandleT handle) {
	if(handle != NULL)
		DeleteElementFromSequence(handle, 0);
}

/* �������, ��������� ��������� ������� ���������� */
extern void LSQ_DeleteRearElement(LSQ_HandleT handle) {
	if(handle != NULL)
		DeleteElementFromSequence(handle, ((SequencePtrT)handle)->logicalSize - 1);
}

/* �������, ��������� ������� ����������, ����������� �������� ����������. ��� ����������� �������� ��������� ��     *
 * ���� ������� � ������� ������.                                                                                    */
extern void LSQ_DeleteGivenElement(LSQ_IteratorT iterator) {
	if(iterator != NULL)
		DeleteElementFromSequence(((IteratorPtrT)iterator)->handle, ((IteratorPtrT)iterator)->index);
}
