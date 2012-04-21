#include "linear_sequence.h"

typedef struct Element{
    LSQ_BaseTypeT data;
    struct Element* nextElement;
    struct Element* previousElement;
}   ElementT, *ElementPtrT;

typedef struct {
    int size;
    ElementPtrT beforeFirst, pastRear;
}   ListT, *ListPtrT;

typedef struct {
    ListPtrT handle;
    ElementPtrT element;
}   IteratorT, *IteratorPtrT;

/* �������, ��������� ������ ���������. ���������� ����������� ��� ���������� */
extern LSQ_HandleT LSQ_CreateSequence(void) {
    ListPtrT handle = (ListPtrT)malloc(sizeof(ListT));
    if(handle == NULL) return LSQ_HandleInvalid;
    
    handle->beforeFirst = (ElementPtrT)malloc(sizeof(ElementT));
    if(handle->beforeFirst == NULL) {
        free(handle);
        return LSQ_HandleInvalid;
    }
    
    handle->pastRear = (ElementPtrT)malloc(sizeof(ElementT));
    if(handle->pastRear == NULL) {
        free(handle->beforeFirst);
        free(handle);
        return LSQ_HandleInvalid;
    }
    
    handle->size = 0;
    handle->beforeFirst->nextElement = handle->pastRear;
    handle->beforeFirst->previousElement = NULL;
    handle->pastRear->nextElement = NULL;
    handle->pastRear->previousElement = handle->beforeFirst;
    
    return handle;
}

/* �������, ������������ ��������� � �������� ������������. ����������� ������������� ��� ������ */
extern void LSQ_DestroySequence(LSQ_HandleT handle) {    
    if(handle == LSQ_HandleInvalid) return;
    IteratorPtrT iterator = LSQ_GetFrontElement(handle);
    
    while(!LSQ_IsIteratorPastRear(iterator)) {
        free(iterator->element->previousElement);
        LSQ_AdvanceOneElement(iterator);
    }
    free(iterator->element->previousElement);
    free(iterator->element);
    free(handle);
    LSQ_DestroyIterator(iterator);
}

/* �������, ������������ ������� ���������� ��������� � ���������� */
extern LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle) {
    if(handle == LSQ_HandleInvalid) return -1;
    return ((ListPtrT)handle)->size;
}

/* ��������� ��� ������� ������� �������� � ������ � ���������� ��� ���������� */
/* �������, ������������ ��������, ����������� �� ������� � ��������� �������� */
extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index) {
    if(handle == LSQ_HandleInvalid) return NULL;
    IteratorPtrT iterator = (IteratorPtrT)LSQ_GetFrontElement(handle);
    LSQ_ShiftPosition(iterator, index);
    return iterator;
}

/* �������, ������������ ��������, ����������� �� ������ ������� ���������� */
extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle) {
    if(handle == LSQ_HandleInvalid) return NULL;
    IteratorPtrT iterator = (IteratorPtrT)malloc(sizeof(IteratorT));
    if(iterator == NULL) return NULL;
    iterator->handle = (ListPtrT)handle;
    iterator->element = ((ListPtrT)handle)->beforeFirst;
    LSQ_AdvanceOneElement(iterator);
    return iterator;
}

/* �������, ������������ ��������, ����������� �� ��������� ������� ���������� */
extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle) {
    if(handle == LSQ_HandleInvalid) return NULL;
    IteratorPtrT iterator = (IteratorPtrT)malloc(sizeof(IteratorT));
    if(iterator == NULL) return NULL;
    iterator->handle = (ListPtrT)handle;
    iterator->element = ((ListPtrT)handle)->pastRear;
    return iterator;
}

/* �������, ������������ �������� � �������� ������������ � ������������� ������������� ��� ������ */
extern void LSQ_DestroyIterator(LSQ_IteratorT iterator) {
    free(iterator);
}

/* �������, ������������, ����� �� ������ �������� ���� ����������� */
extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator) {
    if(iterator == NULL) return 0;
    return !LSQ_IsIteratorPastRear(iterator) && !LSQ_IsIteratorBeforeFirst(iterator);
}

/* �������, ������������, ��������� �� ������ �������� �� �������, ��������� �� ��������� � ���������� */
extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator) {
    if(iterator == NULL) return 0;
    return ((IteratorPtrT)iterator)->element == ((IteratorPtrT)iterator)->handle->pastRear;
}

/* �������, ������������, ��������� �� ������ �������� �� �������, �������������� ������� � ���������� */
extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator) {
    if(iterator == NULL) return 0;
    return ((IteratorPtrT)iterator)->element == ((IteratorPtrT)iterator)->handle->beforeFirst;
}

/* ������� ���������������� ��������. ���������� ��������� �� �������, �� ������� ��������� ������ �������� */
extern LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator) {
    if(iterator == NULL || ((IteratorPtrT)iterator)->handle == LSQ_HandleInvalid
    || !LSQ_IsIteratorDereferencable(iterator)) return NULL;
    return &(((IteratorPtrT)iterator)->element->data);
}

/* �������, ������������ �������� �� ���� ������� ������ */
extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator) {
	LSQ_ShiftPosition(iterator, 1);
}

/* �������, ������������ �������� �� ���� ������� ����� */
extern void LSQ_RewindOneElement(LSQ_IteratorT iterator) {
 	LSQ_ShiftPosition(iterator, -1);
}

/* �������, ������������ �������� �� �������� �������� �� ������ */
extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift) {
    if(iterator == NULL || ((IteratorPtrT)iterator)->handle == LSQ_HandleInvalid) return;
    int i;
    
    if(shift > 0) {
        for(i = 0; i < shift && !LSQ_IsIteratorPastRear(iterator); i++) {
            ((IteratorPtrT)iterator)->element = ((IteratorPtrT)iterator)->element->nextElement;
        }
    }
    else {
        for(i = shift; i < 0 && !LSQ_IsIteratorBeforeFirst(iterator); i++) {
            ((IteratorPtrT)iterator)->element = ((IteratorPtrT)iterator)->element->previousElement;
        }
    }
}

/* �������, ��������������� �������� �� ������� � ��������� ������� */
extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos) {
    if(iterator == NULL) return;
    IteratorPtrT iter = LSQ_GetFrontElement(((IteratorPtrT)iterator)->handle);
    LSQ_ShiftPosition(iter, pos);
    ((IteratorPtrT)iterator)->element = iter->element;
    LSQ_DestroyIterator(iter);
}

/* �������, ����������� ������� � ������ ���������� */
extern void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element) {
    if(handle == LSQ_HandleInvalid) return;
    LSQ_IteratorT iter = LSQ_GetFrontElement(handle);
    LSQ_InsertElementBeforeGiven(iter, element);
    LSQ_DestroyIterator(iter);
}

/* �������, ����������� ������� � ����� ���������� */
extern void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element) {
    if(handle == LSQ_HandleInvalid) return;
    LSQ_IteratorT iter = LSQ_GetPastRearElement(handle);
    LSQ_InsertElementBeforeGiven(iter, element);
    LSQ_DestroyIterator(iter);
}

/* �������, ����������� ������� � ��������� �� �������, ����������� � ������ ������ ����������. �������, �� �������  *
 * ��������� ��������, � ����� ��� �����������, ���������� �� ���� ������� � �����.                                  */
extern void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement) {
    if(iterator == NULL || LSQ_IsIteratorBeforeFirst(iterator)) return;
    IteratorPtrT iter = (IteratorPtrT)iterator;
    ElementPtrT element = (ElementPtrT)malloc(sizeof(ElementT));
    if(element == NULL) return;
    
    element->nextElement = iter->element;
    element->previousElement = iter->element->previousElement;
    element->data = newElement;
    iter->element->previousElement->nextElement = element;
    iter->element->previousElement = element;
    iter->element = element;
    iter->handle->size++;
}

/* �������, ��������� ������ ������� ���������� */
extern void LSQ_DeleteFrontElement(LSQ_HandleT handle) {
    if(handle == LSQ_HandleInvalid) return;
    LSQ_IteratorT iter = LSQ_GetFrontElement(handle);
    if(iter == NULL) return;
    LSQ_DeleteGivenElement(iter);
    LSQ_DestroyIterator(iter);
}

/* �������, ��������� ��������� ������� ���������� */
extern void LSQ_DeleteRearElement(LSQ_HandleT handle) {
    if(handle == LSQ_HandleInvalid) return;
    LSQ_IteratorT iter = LSQ_GetPastRearElement(handle);
    if(iter == NULL) return;
    LSQ_RewindOneElement(iter);
    LSQ_DeleteGivenElement(iter);
    LSQ_DestroyIterator(iter);
}

/* �������, ��������� ������� ����������, ����������� �������� ����������. ��� ����������� �������� ��������� ��     *
 * ���� ������� � ������� ������.                                                                                    */
extern void LSQ_DeleteGivenElement(LSQ_IteratorT iterator) {
    if(iterator == NULL) return;
    IteratorPtrT iter = (IteratorPtrT)iterator;
    if(iter == NULL || !LSQ_IsIteratorDereferencable(iter)) return;
    ElementPtrT previousElement = iter->element->previousElement;
    ElementPtrT nextElement = iter->element->nextElement;
    previousElement->nextElement = nextElement;
    nextElement->previousElement = previousElement;
    free(iter->element);
    iter->element = nextElement;
    iter->handle->size--;
}
