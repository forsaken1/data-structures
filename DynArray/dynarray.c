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

/* Функция, создающая пустой контейнер. Возвращает назначенный ему дескриптор */
extern LSQ_HandleT LSQ_CreateSequence(void) {
    SequencePtrT pointer = (SequencePtrT)malloc(sizeof(SequenceT));
    
    if(pointer == LSQ_HandleInvalid)
        return LSQ_HandleInvalid;            
    pointer->data = (LSQ_BaseTypeT*)malloc(sizeof(LSQ_BaseTypeT) * INITIAL_SEQUENCE_SIZE);
    pointer->physicalSize = INITIAL_SEQUENCE_SIZE;
    pointer->logicalSize = 0;
    return pointer;
}

/* Функция, уничтожающая контейнер с заданным дескриптором. Освобождает принадлежащую ему память */
extern void LSQ_DestroySequence(LSQ_HandleT handle) {    
    if (handle != LSQ_HandleInvalid) {
        free(((SequencePtrT)handle)->data);
        free(handle);
    }
}

/* Функция, возвращающая текущее количество элементов в контейнере */
extern LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle) {
    if (handle == LSQ_HandleInvalid)
        return -1; 
    return ((SequencePtrT)handle)->logicalSize;
}

/* Следующие три функции создают итератор в памяти и возвращают его дескриптор */
/* Функция, возвращающая итератор, ссылающийся на элемент с указанным индексом */
extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index) {	
    return CreateIterator(handle, index);                   
}

/* Функция, возвращающая итератор, ссылающийся на первый элемент контейнера */
extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle) {
	if(handle != NULL)
		return CreateIterator(handle, 0);         
}

/* Функция, возвращающая итератор, ссылающийся на последний элемент контейнера */
extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle) {
    if (handle == LSQ_HandleInvalid)    
        return 0;
    return CreateIterator(handle, ((SequencePtrT)handle)->logicalSize );   
}

/* Функция, уничтожающая итератор с заданным дескриптором и освобождающая принадлежащую ему память */
extern void LSQ_DestroyIterator(LSQ_IteratorT iterator) {
    free(iterator); 
}

/* Функция, определяющая, может ли данный итератор быть разыменован */
extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator) {
    if (iterator == NULL)
        return 0;
    return !LSQ_IsIteratorPastRear(iterator) && !LSQ_IsIteratorBeforeFirst(iterator);
}

/* Функция, определяющая, указывает ли данный итератор на элемент, следующий за последним в контейнере */
extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator) {
    if (iterator == NULL) 
        return 0;
    return ((IteratorPtrT)iterator)->index >= ((SequencePtrT)((IteratorPtrT)iterator)->handle)->logicalSize;
}

/* Функция, определяющая, указывает ли данный итератор на элемент, предшествующий первому в контейнере */
extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator) {
    if (iterator == NULL) 
        return 0;
    return ((IteratorPtrT)iterator)->index < 0;      
}

/* Функция разыменовывающая итератор. Возвращает указатель на элемент, на который ссылается данный итератор */
extern LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator) {
    IteratorPtrT iter = (IteratorPtrT)iterator;
       
    if( iter == NULL || ((SequencePtrT)(iter->handle)) == LSQ_HandleInvalid )
        return NULL;
    return ((SequencePtrT)(iter->handle))->data + iter->index;
}

/* Функция, перемещающая итератор на один элемент вперед */
extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator) {
	if(iterator != NULL)
		LSQ_ShiftPosition(iterator, 1);
}

/* Функция, перемещающая итератор на один элемент назад */
extern void LSQ_RewindOneElement(LSQ_IteratorT iterator) {
	if(iterator != NULL)
		LSQ_ShiftPosition(iterator, -1);
}

/* Функция, перемещающая итератор на заданное смещение со знаком */
extern void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift) {
    if(iterator != NULL) 
        ((IteratorPtrT)iterator)->index += shift;
}

/* Функция, устанавливающая итератор на элемент с указанным номером */
extern void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos) {
    if(iterator != NULL) 
        ((IteratorPtrT)iterator)->index = pos;
}

/* Функция, добавляющая элемент в начало контейнера */
extern void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element) {
    if(handle != NULL)
		InsertElementInSequence(handle, element, 0);
}

/* Функция, добавляющая элемент в конец контейнера */
extern void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element) {
    if(handle != NULL)
		InsertElementInSequence(handle, element, ((SequencePtrT)handle)->logicalSize);
}

/* Функция, добавляющая элемент в контейнер на позицию, указываемую в данный момент итератором. Элемент, на который  *
 * указывает итератор, а также все последующие, сдвигается на одну позицию в конец.                                  */
extern void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement) {
    if(iterator != NULL)
		InsertElementInSequence(((IteratorPtrT)iterator)->handle, newElement, ((IteratorPtrT)iterator)->index);
}

/* Функция, удаляющая первый элемент контейнера */
extern void LSQ_DeleteFrontElement(LSQ_HandleT handle) {
	if(handle != NULL)
		DeleteElementFromSequence(handle, 0);
}

/* Функция, удаляющая последний элемент контейнера */
extern void LSQ_DeleteRearElement(LSQ_HandleT handle) {
	if(handle != NULL)
		DeleteElementFromSequence(handle, ((SequencePtrT)handle)->logicalSize - 1);
}

/* Функция, удаляющая элемент контейнера, указываемый заданным итератором. Все последующие элементы смещаются на     *
 * одну позицию в сторону начала.                                                                                    */
extern void LSQ_DeleteGivenElement(LSQ_IteratorT iterator) {
	if(iterator != NULL)
		DeleteElementFromSequence(((IteratorPtrT)iterator)->handle, ((IteratorPtrT)iterator)->index);
}
