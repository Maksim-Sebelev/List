#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "lib/lib.hpp"
#include "list/list.hpp"
#include "list/err_parse/err_parse.hpp"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t       GetFree          (const List_t* list);
static size_t       GetCapacity      (const List_t* list);
static size_t       GetDataSize      (const List_t* list);
static list_elem_t   GetDataElem      (const List_t* list, size_t Data_i);
static size_t       GetNextIndex     (const List_t* list, size_t NowIndex);
static size_t       GetPrevIndex     (const List_t* list, size_t NowIndex);
static size_t       GetNextInCtor      (const List_t* list, size_t Data_i);
static size_t       GetPrevInCtor      (const List_t* list, size_t Data_i);

static void ListElemCtor                  (ListElem* elem, const list_elem_t value, const size_t next, const size_t prev);
static bool IsListFull                    (const List_t* list);
static bool IsListEmpty                   (const List_t* list);

static void GraphicDumpHelper  (const List_t* list, const char* File, const int Line, const char* Func);
static void DotBegin           (FILE* dotFile);
static void DotEnd             (FILE* dotFile);
static void DotCreateDumpPlace (FILE* dotFile, const char* File, const int Line, const char* Func);
static void DotCreateNotFreeNode      (FILE* dotFile, const List_t* list, const size_t node_i);
static void DotCreateAllNodes  (FILE* dotFile, const List_t* list);
static void DotCreateNextEdges (FILE* dotFile, const List_t* list);
static void DotCreatePrevEdges (FILE* dotFile, const List_t* list);
static void DotCreateRestList  (FILE* dotFile, const List_t* list);
static void DotCreateEdges     (FILE* dotFile, const List_t* list);


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const list_elem_t Poison = 666;

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListError_t ListCtor(List_t* list, size_t capacity)
{
    assert(list);

    list->capacity    = capacity + 1;
    list->size        = 0;
    list->data        = (ListElem*) calloc(capacity + 1, sizeof(ListElem));

    list->data = nullptr; //// !!!!!!!!!!

    if (!list->data)
    {
        return GET_STATUS_WARN(ListWarningType::FAILED_REALLOCATE_DATA_AFTER_ERASE);
        // return GET_STATUS_ERR(ListErrorType::FAILED_ALLOCATE_MEMORY_IN_CTOR);
    }

    list->data[GetTail(list)].prev = list->data[GetTail(list)].prev;
    list->data[0]            .next = list->data[GetTail(list)].next;
    list->data[GetTail(list)].next = 0;
    list->free                     = 1;

    ListElemCtor(&list->data[0], Poison, 0, 0); 

    for (size_t Data_i = 1; Data_i < GetCapacity(list); Data_i++)
    {
        ListElemCtor(&list->data[Data_i], Poison, GetNextInCtor(list, Data_i), GetPrevInCtor(list, Data_i));
    }

    return GET_STATUS_OK();

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ListError_t ListDtor(List_t* list)
{
    ListElem* data = list->data;

    if (!data)
        return GET_STATUS_ERR(ListErrorType::TRY_TO_DTOR_NULLPTR_DATA);

    FREE(list->data);

    *list = {};

    return GET_STATUS_OK();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ListError_t InsertAfter(List_t* list, const size_t RefElem, const list_elem_t InsertElem, size_t* InsertPlace)
// {
//     assert(list);

//     ListError_t err = {};

//     if (IsListFull(list))
//     {
//         err.IsFatalError = 1;
//         err.ListOverFlow = 1;
//         return LIST_VERIF(list, err);
//     }

//     list->size++;
 
//     size_t free       = GetFree(list);
//     size_t PrevFree   = GetPrevIndex(list, free);
//     size_t NextFree   = GetNextIndex(list, free);
//     size_t NextRef    = GetNextIndex(list, RefElem);

//     *InsertPlace               = free;
//     list->data[free]    .value = InsertElem;
//     list->data[RefElem] .next  = free;
//     list->data[NextRef] .prev  = free;
//     list->data[free]    .prev  = RefElem;
//     list->data[free]    .next  = NextRef;
//     list->free                 = NextFree;
//     list->data[NextFree].prev  = PrevFree;
//     list->data[PrevFree].next  = NextFree;

//     return LIST_VERIF(list, err);
// }

// //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ListError_t InsertBefore(List_t* list, const size_t RefElem, const list_elem_t InsertElem, size_t* InsertPlace)
// {
//     assert(list);

//     ListError_t err = {};

//     if (IsListFull(list))
//     {
//         err.IsFatalError = 1;
//         err.ListOverFlow = 1;
//         return LIST_VERIF(list, err);
//     }

//     list->size++;

//     size_t free       = GetFree(list);
//     size_t PrevFree   = GetPrevIndex(list, free);
//     size_t NextFree   = GetNextIndex(list, free);
//     size_t PrevRef    = GetPrevIndex(list, RefElem);

//     *InsertPlace              = free;
//     list->data[free]    .value = InsertElem;
//     list->data[free]    .next = RefElem;
//     list->data[free]    .prev = PrevRef;
//     list->data[RefElem] .prev = free;
//     list->data[PrevRef] .next = free;
//     list->free                = NextFree;
//     list->data[NextFree].prev = PrevFree;
//     list->data[PrevFree].next = NextFree;

//     return LIST_VERIF(list, err);
// }

// //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ListError_t Erase(List_t* list, const size_t ErasePlace, list_elem_t* EraseElem)
// {
//     assert(list);

//     ListError_t err = {};

//     if (IsListEmpty(list))
//     {
//         err.IsFatalError   = 1;
//         err.ListUnderFlow = 1;
//         return LIST_VERIF(list, err);
//     }

//     list->size--;

//     size_t free      = GetFree(list);
//     size_t PrevFree  = GetPrevIndex(list, free);
//     size_t NextFree  = GetNextIndex(list, free);
//     size_t PrevErase = GetPrevIndex(list, ErasePlace);
//     size_t NextErase = GetNextIndex(list, ErasePlace);

//     *EraseElem                  = GetDataElem(list, ErasePlace);
//     list->data[ErasePlace].value = Poison;

//     list->data[ErasePlace].next = free;
//     list->data[ErasePlace].prev = PrevFree;
//     list->data[PrevErase] .next = NextErase;
//     list->data[NextErase] .prev = PrevErase;
//     list->data[free]      .prev = free;
//     list->free                  = ErasePlace;
//     list->data[PrevFree]  .next = ErasePlace;
//     list->data[NextFree]  .prev = ErasePlace;
    
//     return LIST_VERIF(list, err);
// }

// //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ListError_t PushBack(List_t* list, const list_elem_t PushElem, size_t* PushPlace)
// {
//     assert(list);

//     ListError_t err = {};

//     if (IsListFull(list))
//     {
//         err.IsFatalError = 1;
//         err.ListOverFlow = 1;
//         return LIST_VERIF(list, err);
//     }


//     InsertAfter(list, GetTail(list), PushElem, PushPlace);

//     return LIST_VERIF(list, err);
// }

// //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ListError_t PushFront(List_t* list, const list_elem_t PushElem, size_t* PushPlace)
// {
//     assert(list);

//     ListError_t err = {};

//     if (IsListFull(list))
//     {
//         err.IsFatalError = 1;
//         err.ListOverFlow = 1;
//         return LIST_VERIF(list, err);
//     }

//     InsertBefore(list, GetHead(list), PushElem, PushPlace);

//     return LIST_VERIF(list, err);
// }

// //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ListError_t PopBack(List_t* list, list_elem_t* PopElem)
// {
//     assert(list);

//     ListError_t err = {};

//     if (IsListEmpty(list))
//     {
//         err.IsFatalError = 1;
//         err.ListUnderFlow = 1;
//         return LIST_VERIF(list, err);
//     }

//     Erase(list, GetTail(list), PopElem);

//     return LIST_VERIF(list, err);
// }

// //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ListError_t PopFront(List_t* list, list_elem_t* PopElem)
// {
//     assert(list);

//     ListError_t err = {};

//     if (IsListEmpty(list))
//     {
//         err.IsFatalError = 1;
//         err.ListUnderFlow = 1;
//         return LIST_VERIF(list, err);
//     }

//     Erase(list, GetHead(list), PopElem);

//     return LIST_VERIF(list, err);
// }

// //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void ListElemCtor(ListElem* elem, const list_elem_t value, const size_t next, const size_t prev)
{
    assert(elem);
    elem->value = value;
    elem->next  = next;
    elem->prev  = prev;
    return;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// static list_elem_t GetDataElem(const List_t* list, size_t Data_i)
// {
//     assert(list);
//     return list->data[Data_i].value;
// }

// //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

size_t GetTail(const List_t* list)
{
    assert(list);
    return list->data[0].prev;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// static size_t GetNextIndex(const List_t* list, size_t NowIndex)
// {
//     assert(list);   
//     return list->data[NowIndex].next;
// }

// //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// static size_t GetPrevIndex(const List_t* list, size_t NowIndex)
// {
//     assert(list);
//     return list->data[NowIndex].prev;
// }

// //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

size_t GetHead(const List_t* list)
{
    assert(list);
    return list->data[0].next;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetFree(const List_t* list)
{
    assert(list);
    return list->free;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetCapacity(const List_t* list)
{
    assert(list);
    return list->capacity;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetDataSize(const List_t* list)
{
    assert(list);
    return list->size;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// static bool IsListFull(const List_t* list)
// {
//     assert(list);
//     return GetCapacity(list) == GetDataSize(list) + 1;
// }

// //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// static bool IsListEmpty(const List_t* list)
// {
//     assert(list);
//     return GetDataSize(list) == 0;
// }

// //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetNextInCtor(const List_t* list, size_t Data_i)
{
    assert(list);

    size_t temp_next = (Data_i + 1) % (GetCapacity(list));
    return temp_next ? temp_next : 1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static size_t GetPrevInCtor(const List_t* list, size_t Data_i)
{
    assert(list);
    
    size_t temp_prev = (Data_i - 1) % (GetCapacity(list)); 
    return temp_prev ? temp_prev : GetCapacity(list) - 1;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ListError_t Verif(List_t* list, ListError_t* err, const char* File, const int Line, const char* Func)
// {
//     assert(list);
//     assert(File);
//     assert(Func);

//     ErrPlaceCtor(err, File, Line, Func);

//     return *err;
// }

// //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

