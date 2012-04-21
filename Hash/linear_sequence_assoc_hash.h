
#ifndef LINEAR_SEQUENCE_H
#define LINEAR_SEQUENCE_H

#include <stdlib.h>

/* ��� �������� � ���������� �������� */
typedef void* LSQ_BaseTypeT;
/* ��� ����� ���������� */
typedef void* LSQ_KeyT;

typedef int LSQ_SizeT;

typedef void* LSQ_Callback_CloneFuncT (void*);
typedef size_t LSQ_Callback_SizeFuncT (void*);
typedef int LSQ_Callback_CompareFuncT (void*, void*);

/* ���������� ���������� */
typedef void* LSQ_HandleT;

/* �������������������� �������� ����������� ���������� */
#define LSQ_HandleInvalid NULL

/* ���������� ��������� */
typedef void* LSQ_IteratorT;


/* �������, ��������� ������ ���������. ���������� ����������� ��� ���������� */
extern LSQ_HandleT LSQ_CreateSequence(LSQ_Callback_CloneFuncT keyCloneFunc, LSQ_Callback_SizeFuncT keySizeFunc, LSQ_Callback_CompareFuncT keyCompFunc,
                                      LSQ_Callback_CloneFuncT valCloneFunc);
/* �������, ������������ ��������� � �������� ������������. ����������� ������������� ��� ������ */
extern void LSQ_DestroySequence(LSQ_HandleT handle);

/* �������, ������������ ������� ���������� ��������� � ���������� */
extern LSQ_SizeT LSQ_GetSize(LSQ_HandleT handle);

/* �������, ������������, ����� �� ������ �������� ���� ����������� */
extern int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator);
/* �������, ������������, ��������� �� ������ �������� �� �������, ��������� �� ��������� � ���������� */
extern int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator);
/* �������, ������������, ��������� �� ������ �������� �� �������, �������������� ������� � ���������� */
extern int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator);

/* ������� ���������������� ��������. ���������� ��������� �� �������� ��������, �� ������� ��������� ������ �������� */
extern LSQ_BaseTypeT LSQ_DereferenceIterator(LSQ_IteratorT iterator);
/* ������� ���������������� ��������. ���������� ��������� �� ���� ��������, �� ������� ��������� ������ �������� */
extern LSQ_KeyT LSQ_GetIteratorKey(LSQ_IteratorT iterator);

/* ��������� ��� ������� ������� �������� � ������ � ���������� ��� ���������� */
/* �������, ������������ ��������, ����������� �� ������� � ��������� ������. ���� ������� � ������ ������  *
 * ����������� � ����������, ������ ���� ��������� �������� PastRear.                                       */
extern LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_KeyT key);
/* �������, ������������ ��������, ����������� �� ������ ������� ���������� */
extern LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle);
/* �������, ������������ ��������, ����������� �� ��������� �������, ��������� �� ��������� ��������� ���������� */
extern LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle);

/* �������, ������������ �������� � �������� ������������ � ������������� ������������� ��� ������ */
extern void LSQ_DestroyIterator(LSQ_IteratorT iterator);

/* ��������� ������� ��������� ����������� �������� �� ���������. ��� ���� �������������� ������ ������  *
 * �� ��� ������, ������� ���� � ����������.                                                             */
/* �������, ������������ �������� �� ���� ������� ������ */
extern void LSQ_AdvanceOneElement(LSQ_IteratorT iterator);

/* �������, ����������� ����� ���� ����-�������� � ���������. ���� ������� � ������ ������ ����������,  *
 * ��� �������� ����������� ���������.                                                                  */
extern void LSQ_InsertElement(LSQ_HandleT handle, LSQ_KeyT key, LSQ_BaseTypeT value);

/* �������, ��������� ������� ����������, ����������� �������� ������. */
extern void LSQ_DeleteElement(LSQ_HandleT handle, LSQ_KeyT key);

#endif
